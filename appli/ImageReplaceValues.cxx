
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"
#include "itkImageFileReader.h"

// Software Guide : EndCodeSnippet

using namespace std;

void help(char* exec){
    cerr<<"Replace all non 0 values with the given label"<<endl;
    cerr<<"Usage: "<<exec<<" -f <image filename> -o <output filename> -label <int> ex: "<<exec<<" -f img.nii.gz -o imgout.nii.gz -label 255"<<endl;
}

const unsigned int InputDimension = 3;
typedef double ReaderPixelType;
typedef itk::Image< ReaderPixelType, InputDimension > ReaderImageType;
typedef unsigned short PixelType;
typedef itk::Image< PixelType, InputDimension > ImageType;

int main( int argc, char ** argv )
{

  string outputFileName = "";
  string filename = "";
  int label = 0;

  for(int i = 1; i < argc - 1; i++){
      string param = string(argv[i]);
      if(param == "-o"){
          outputFileName = string(argv[i+1]);
      }else if(param == "-f"){
          filename = string(argv[i+1]);
      }else if(param == "-label"){
          label = atoi(argv[i+1]);
      }
  }
  if(filename == "" || outputFileName == ""){
      help(argv[0]);
      return 0;
  }

  typedef unsigned short PixelType;
  typedef itk::Image< PixelType, InputDimension > ImageType;

    typedef itk::ImageFileWriter< ImageType > ImageWriterType;
    ImageType::Pointer resimage = 0;


    typedef  itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( filename.c_str() );
    reader->Update();

    try
    {
        resimage = reader->GetOutput();

    }catch (itk::ExceptionObject &excp)
    {
    std::cerr << "Exception thrown while reading the image. " << filename <<std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

    typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
    ThresholdType::Pointer threshold = ThresholdType::New();

    threshold->ThresholdAbove(0);
    threshold->SetOutsideValue(label);
    threshold->SetInput(resimage);
    threshold->Update();
    resimage = threshold->GetOutput();



  cout<<endl<<"{\"outputFilename\": \""<<outputFileName<<"\"}"<<endl;
  ImageWriterType::Pointer writer = ImageWriterType::New();

  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( resimage );
  writer->Update();
  // Software Guide : EndCodeSnippet


  return EXIT_SUCCESS;
}

