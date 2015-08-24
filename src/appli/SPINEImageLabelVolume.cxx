
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkNormalizeImageFilter.h"
#include "itkImageFileWriter.h"


using namespace std;

void help(char* exec){
    cerr<<"Calculates the number of voxels per label in a labeled volume. The output is a JSON doc with the volume of each structure."<<endl;
    cerr<<"The output is: { <labelNumber> : <volumeValue> }, e.x. { \"0\" : 40 }"<<endl;
    cerr<<"How to use: "<<string(exec)<<" -f <filename>"<<endl;
    cerr<<"Using the standard input:"<<endl;
    cerr<<"cat <imageFilename> | "<<string(exec)<<endl;
}

int main( int argc, char ** argv )
{

  string filename = "";

  //string imagestring = "";
  for(int i = 1; i < argc; i++){
      string param = string(argv[i]);
      if(param.compare("-f") == 0){
          filename = string(argv[i+1]);
      }else if(param.compare("-h") == 0 || param.compare("-help") == 0){
          help(argv[0]);
          return 0;
      }
  }

  typedef unsigned short PixelType;
  static const int dimension = 3;

  bool isTempFile = false;

  if(filename.compare("") == 0){
      char c;
      string str = "";
      while(cin.get(c)){
          str += c;
      }

      char buffer[] = "/tmp/SPINEXXXXXXX";
      mktemp(buffer);
      filename = string(buffer);
      filename.append(".nii.gz");

      ofstream tempFile;
      tempFile.open (filename.c_str());
      tempFile << str;
      tempFile.close();
      isTempFile = true;
  }

  typedef itk::Image< PixelType, dimension > ImageType;
  typedef  itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename.c_str() );
  reader->Update();

  if(isTempFile){
      remove(filename.c_str());
  }

  ImageType::Pointer image = 0;

  try{
        image = reader->GetOutput();
  }catch (itk::ExceptionObject &excp)
  {
  cerr << "Exception thrown while reading the image. " << filename <<endl;
  cerr << excp << endl;
  return EXIT_FAILURE;
  }

  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType it(image, image->GetLargestPossibleRegion());
  it.GoToBegin();

  map<PixelType, int>  allImageLabels;

  while(!it.IsAtEnd()){
      if(it.Get() != 0){
          if(allImageLabels.find(it.Get()) == allImageLabels.end()){
              allImageLabels[it.Get()] = 0;
          }
          allImageLabels[it.Get()] = allImageLabels[it.Get()] + 1;
      }
      ++it;
  }

  cout<<"{"<<endl;
  int n = 0;
  int size = allImageLabels.size();
  ImageType::SpacingType spacing = image->GetSpacing();
  float spcvol = spacing[0]*spacing[1]*spacing[2];
  cout<<std::fixed;
//  cout<<"\"spacing\" : ["<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<"],"<<endl;

  for (map<PixelType, int>::iterator it=allImageLabels.begin(); it!=allImageLabels.end(); ++it){

      float volume = it->second * spcvol;
      cout << "\""<< it->first << "\" : " <<volume<<endl;

      if(n < size -1){
          cout<<",";
      }
      cout<<endl;
      n++;
  }

  cout<<"}"<<endl;


  return EXIT_SUCCESS;
}




