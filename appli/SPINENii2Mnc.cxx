
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"


using namespace std;

void help(char* exec){
    cerr<<"Resample an image to RAS space, use this program to use with xtk tools."<<endl;
    cerr<<"Usage: "<<exec<<" -f <input filename .nii.gz or .nii>"<<endl;
    cerr<<"Output: <filename>.mnc"<<endl;
    cerr<<"options: "<<endl;
    cerr<<"-o <outputfilename>"<<endl;
}

int main( int argc, char ** argv )
{

  string filename = "";
  string outfilename = "";

  for(int i = 1; i < argc - 1; i++){
      string param = string(argv[i]);
      if(param.compare("-f") == 0){
          filename = string(argv[i+1]);
      }else if(param.compare("-o") == 0){
          outfilename = string(argv[i+1]);
      }


  }

  if(filename.compare("") == 0){
      help(argv[0]);
      return EXIT_FAILURE;
  }

  typedef unsigned short PixelType;
  static const int dimension = 3;

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
  std::cerr << "Exception thrown while reading the image. " << filename <<std::endl;
  std::cerr << excp << std::endl;
  return EXIT_FAILURE;
  }

  if(outfilename.compare("") == 0){
      if(filename.find_last_of(".nii.gz")  != string::npos ){
          outfilename = filename.substr(0, filename.find_last_of(".nii.gz") - 6);
      }else if(filename.find_last_of(".nii") != string::npos){
          outfilename = filename.substr(0, filename.find_last_of(".nii") - 3);
      }else{
          outfilename = filename.substr(0, filename.find_last_of(".") - 1);
      }
      outfilename.append(".mnc");
  }



  typedef  itk::ImageFileWriter<ImageType> ImageWriterType;
  cout<<endl<<"{\"outputFilename\": \""<<outfilename<<"\"}"<<endl;
  ImageWriterType::Pointer writer = ImageWriterType::New();

  writer->SetFileName( outfilename.c_str() );
  writer->SetInput( image );
  writer->Update();
  // Software Guide : EndCodeSnippet


  return EXIT_SUCCESS;
}

