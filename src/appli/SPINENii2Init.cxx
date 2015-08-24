
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
    cerr<<"Converts a nifti image with the same size as the input image. The input image must be provided by the standart input, the output also goes to the standard output"<<endl;
    cerr<<"ex. cat <imageFilename> | "<<string(exec)<<endl;
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

  while(!it.IsAtEnd()){
      it.Set(0);
      ++it;
  }

  if(!isTempFile){
      char buffer[] = "/tmp/SPINEXXXXXXX";
      mktemp(buffer);
      filename = string(buffer);
      filename.append(".nii.gz");
  }

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(filename.c_str());
  writer->SetInput(image);
  writer->Update();

  ifstream tempInFile(filename.c_str());
  char c;
  while(tempInFile.get(c)){
      cout<<c;
  }
  tempInFile.close();
  remove(filename.c_str());
  return EXIT_SUCCESS;
}
