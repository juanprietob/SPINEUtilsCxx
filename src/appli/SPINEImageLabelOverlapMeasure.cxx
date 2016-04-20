#include "itkLabelOverlapMeasuresImageFilter.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"

using namespace std;

void help(char* exec){
    cerr<<"Calculates several label overlap measures between two images. The first image as argument is the source image and the second one is the target image. The output is a JSON doc with values of the dice coefficient, the jaccard coefficient, the false negative error, the false positive error, the mean overlap, the target overlap, the union overlap and the volumes similarity."<<endl;
    cerr<<"How to use: "<<string(exec)<<" -src <SourceFilename> -trgt <TargetFilename>"<<endl;
}

int main( int argc, char ** argv )
{
  string referenceVolume = "";
  string comparedVolume = "";
  string sep = "";
  string referenceFileName = "";
  string comparedFileName = "";

  for(int i = 1; i < argc; i++){
      string param = string(argv[i]);
      if(param.compare("-src") == 0 || param.compare("-source") == 0){
          referenceVolume = string(argv[i+1]);
      } else if(param.compare("-trgt") == 0 || param.compare("-target") == 0) {
          comparedVolume = string(argv[i+1]);
      } else if(param.compare("-sep") == 0) {
          sep = string(argv[i+1]);
      } else if(param.compare("-h") == 0 || param.compare("-help") == 0){
          help(argv[0]);
          return 0;
      }
  }

  typedef unsigned short PixelType;
  static const int dimension = 3;
  typedef itk::Image< PixelType, dimension > ImageType;
	typedef  itk::ImageFileReader<ImageType> ReaderType;

  bool isTempReferenceFile = false;
  bool isTempTargetFile = false;

  if(referenceVolume.compare("") != 0 && comparedVolume.compare("") != 0) {

    referenceFileName = referenceVolume;
    comparedFileName = comparedVolume;

  } else {

    char c;
    string str = "";
    while(cin.get(c)){
        str += c;
    }

    if(str.find(sep) != 0) {
      string tempstr = str.substr(0, str.find(sep));

      char buffer[] = "/tmp/SPINEXXXXXXX";
      mktemp(buffer);
      referenceFileName = string(buffer);
      referenceFileName.append(".nii.gz");

      ofstream tempFile;
      tempFile.open (referenceFileName.c_str());
      tempFile << tempstr;
      tempFile.close();
      isTempReferenceFile = true;

      str = str.substr(str.find(sep) + sep.length());
    }

    char buffer[] = "/tmp/SPINEXXXXXXX";
    mktemp(buffer);
    comparedFileName = string(buffer);
    comparedFileName.append(".nii.gz");

    ofstream tempFile;
    tempFile.open (comparedFileName.c_str());
    tempFile << str;
    tempFile.close();
    isTempTargetFile = true;

  }

  ReaderType::Pointer referenceReader = ReaderType::New();
  referenceReader->SetFileName( referenceFileName.c_str() );
  referenceReader->Update();

  if(isTempReferenceFile){
      remove(referenceFileName.c_str());
  }

	ImageType::Pointer referenceImage = 0;

	try{
	    referenceImage = referenceReader->GetOutput();
	}catch (itk::ExceptionObject &excp)
	{
		cerr << "Exception thrown while reading the image. " << referenceFileName <<endl;
		cerr << excp << endl;
		return EXIT_FAILURE;
	}

	ReaderType::Pointer comparedReader = ReaderType::New();
	comparedReader->SetFileName( comparedFileName.c_str() );
	comparedReader->Update();

  if(isTempTargetFile){
      remove(comparedFileName.c_str());
  }

	ImageType::Pointer comparedImage = 0;

	try{
	    comparedImage = comparedReader->GetOutput();
	}catch (itk::ExceptionObject &excp)
	{
		cerr << "Exception thrown while reading the image. " << comparedVolume <<endl;
		cerr << excp << endl;
		return EXIT_FAILURE;
	}

  	typedef itk::ImageRegionIterator<ImageType> IteratorType;
  	map<PixelType, int>  allImageLabels;

  	IteratorType referenceIt(referenceImage, referenceImage->GetLargestPossibleRegion());
  	referenceIt.GoToBegin();

  	while(!referenceIt.IsAtEnd()){
  	    if(allImageLabels.find(referenceIt.Get()) == allImageLabels.end()){
  	        allImageLabels[referenceIt.Get()] = 0;
  	    }
  	    allImageLabels[referenceIt.Get()] = allImageLabels[referenceIt.Get()] + 1;
  	    ++referenceIt;
  	}

   	IteratorType comparedIt(comparedImage, comparedImage->GetLargestPossibleRegion());
  	comparedIt.GoToBegin();

  	while(!comparedIt.IsAtEnd()){
  	    if(allImageLabels.find(comparedIt.Get()) == allImageLabels.end()){
  	        allImageLabels[comparedIt.Get()] = 0;
  	    }
  	    allImageLabels[comparedIt.Get()] = allImageLabels[comparedIt.Get()] + 1;
  	    ++comparedIt;
  	}

    float temporig[3];
    temporig[0] = 0;
    temporig[1] = 0;
    temporig[2] = 0;
    referenceImage->SetOrigin(temporig);
    comparedImage->SetOrigin(temporig);

	typedef itk::LabelOverlapMeasuresImageFilter< ImageType > LabelOverlapMeasuresImageFilter;
	LabelOverlapMeasuresImageFilter::Pointer filter = LabelOverlapMeasuresImageFilter::New();
	filter->SetSourceImage(referenceImage);
	filter->SetTargetImage(comparedImage);
	filter->Update();

  cout<<"{"<<endl;
  int n = 0;
  int size = allImageLabels.size();
  cout<<std::fixed;

  for (map<PixelType, int>::iterator it=allImageLabels.begin(); it!=allImageLabels.end(); ++it){
  	if(it->first != 0) {
      cout << "\""<< it->first << "\" : {";
      float diceCoefficient = filter->GetDiceCoefficient(it->first);
      cout<<"\"dice_coefficient\" : "<<diceCoefficient<<","<<endl;
      float jaccardCoefficient = filter->GetJaccardCoefficient(it->first);
      cout<<"\"jaccard_coefficient\" : "<<jaccardCoefficient<<","<<endl;
      float false_negative_error = filter->GetFalseNegativeError(it->first);
      cout<<"\"false_negative_error\" : "<<false_negative_error<<","<<endl;
      float false_positive_error = filter->GetFalsePositiveError(it->first);
      cout<<"\"false_positive_error\" : "<<false_positive_error<<","<<endl;
      float mean_overlap = filter->GetMeanOverlap(it->first);
      cout<<"\"mean_overlap\" : "<<mean_overlap<<","<<endl;
      float target_overlap = filter->GetTargetOverlap(it->first);
      cout<<"\"target_overlap\" : "<<target_overlap<<","<<endl;
      float union_overlap = filter->GetUnionOverlap(it->first);
      cout<<"\"union_overlap\" : "<<union_overlap<<","<<endl;
      float volume_similarity = filter->GetVolumeSimilarity(it->first);
      cout<<"\"volume_similarity\" : "<<volume_similarity<<endl;
      cout<<"}";
      if(n < size -2){
          cout<<",";
      }
      cout<<endl;
      n++;
  	}
  }

  cout<<"}"<<endl;

  return EXIT_SUCCESS;
}
