
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkNormalizeImageFilter.h"
#include "itkOrientImageFilter.h"

using namespace std;

void help(char* exec){
    cerr<<"Converts a nifti image to BrainBrowser format. The input image must be provided by the standart input,"<<endl;
    cerr<<"ex. cat image.nii.gz | "<<string(exec)<<" -h"<<endl;
    cerr<<"Usage: "<<exec<<endl;
    cerr<<"options: "<<endl;
    cerr<<"-h   Prints the header information in BB format to the standart output"<<endl;
    cerr<<"-c   Prints the content of the file to the standart output"<<endl;
}

int main( int argc, char ** argv )
{

  string filename = "";
  bool printHeader = false;
  bool printContent = false;

  for(int i = 1; i < argc; i++){
      string param = string(argv[i]);
      if(param.compare("-f") == 0){
          filename = string(argv[i+1]);
      }else if(param.compare("-h") == 0){
          printHeader = true;
      }else if(param.compare("-c") == 0){
          printContent = true;
      }
  }

  if(!printHeader && !printContent){
      help(argv[0]);
      return EXIT_FAILURE;
  }

  //string imagestring = "";
  char c;
  string str = "";
  while(cin.get(c)){
      str += c;
  }

  typedef unsigned short PixelType;
  static const int dimension = 3;

  char buffer[] = "/tmp/SPINEXXXXXXX";
  mktemp(buffer);
  filename = string(buffer);
  filename.append(".nii.gz");

  ofstream tempFile;
  tempFile.open (filename.c_str());
  tempFile << str;
  tempFile.close();

  typedef itk::Image< PixelType, dimension > ImageType;
  typedef  itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename.c_str() );
  reader->Update();



  ImageType::Pointer image = 0;

  try{
        image = reader->GetOutput();
  }catch (itk::ExceptionObject &excp)
  {
  cerr << "Exception thrown while reading the image. " << filename <<endl;
  cerr << excp << endl;
  return EXIT_FAILURE;
  }

  typedef unsigned short OutputPixelType;
  typedef itk::Image< OutputPixelType, dimension > OutputImageType;
  OutputImageType::Pointer outimage = image;

//  itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter = itk::OrientImageFilter<ImageType,ImageType>::New();
//  orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS);
//  orienter->SetUseImageDirection(true);

//  orienter->SetInput(outimage);
//  orienter->Update();
//  outimage = orienter->GetOutput();

  /*typedef itk::CastImageFilter<ImageType, OutputImageType > CastImageType;
  CastImageType::Pointer cast = CastImageType::New();
  cast->SetInput(image);
  cast->Update();
  OutputImageType::Pointer outimage = cast->GetOutput();

  typedef itk::RescaleIntensityImageFilter< ImageType, OutputImageType > RescaleType;
  RescaleType::Pointer rescale = RescaleType::New();
  rescale->SetInput( image );
  rescale->SetOutputMinimum( 0 );
  rescale->SetOutputMaximum( itk::NumericTraits< OutputPixelType >::max() );
  rescale->Update();
  outimage = rescale->GetOutput();*/


  //OutputImageType::Pointer outimage = image;
  //cerr<<outimage<<endl;


  if(printHeader){

      OutputImageType::DirectionType direction = outimage->GetDirection();
      OutputImageType::SizeType size = outimage->GetLargestPossibleRegion().GetSize();
      OutputImageType::PointType origin = outimage->GetOrigin();
      OutputImageType::SpacingType spacing = outimage->GetSpacing();

      vector<string> order;
      order.push_back("zspace");
      order.push_back("yspace");
      order.push_back("xspace");

      vector<unsigned> orderindex;
      orderindex.push_back(2);
      orderindex.push_back(1);
      orderindex.push_back(0);

      cout<<"{";
      cout<<"\"order\" : [";
      for(int i = 0; i < 3; i++){
          cout<<"\"" + order[i] + "\"";
          if(i < 2){
              cout<<",";
          }
      }
      cout<<"],";

      for(unsigned i = 0; i < order.size(); i++){
          cout<<"\"" + order[i] + "\" : {";
          cout<<"\"direction_cosines\" : [";
          for(unsigned j = 0; j < 3; j++){
              cout<<direction[orderindex[i]][j];
              if(j < 2){
                  cout<<",";
              }
          }
          cout<<"],";
          cout<<"\"space_length\" : "<<size[orderindex[i]]<<",";
          cout<<"\"start\" : "<<origin[orderindex[i]]<<",";
          cout<<"\"step\" : " <<spacing[orderindex[i]];
          cout<<"}";
          //if(i < order.size() - 1){
              cout<<",";
          //}
      }
      cout<<"\"type\" : \"USHORT\""<<endl;
      cout<<"}"<<endl;
  }

  if(printContent){
      typedef itk::ImageRegionIterator<OutputImageType> IteratorType;
      IteratorType it(outimage, outimage->GetLargestPossibleRegion());
      it.GoToBegin();
      int numt = sizeof(OutputPixelType);
      //cerr<<"numt:"<<numt<<endl;
      char *v = new char[numt];
      while(!it.IsAtEnd()){
          OutputPixelType p = it.Get();
          char* vpoint = (char*)(&p);
          for(int i = 0; i < numt; i++){
              v[i] = *vpoint;
              vpoint++;
          }
          cout.write(const_cast<char*>(v), numt);
          ++it;
      }
      delete v;
  }
  remove(filename.c_str());
  return EXIT_SUCCESS;
}


