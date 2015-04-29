#include "itkLabelVotingImageFilter.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include <vector>

using namespace std;

void help(char* exec){
    cerr<<"Calculates voting from several images."<<endl;
    cerr<<"How to use: "<<string(exec)<<" <Filename 1> <Filename 2> ... <Filename N>"<<endl;
}

int main( int argc, char ** argv )
{

  if(argc < 2){
      help(argv[0]);
      return 0;
  }

  typedef unsigned short PixelType;
  static const int dimension = 3;
  typedef itk::Image< PixelType, dimension > ImageType;

  PixelType labelforundecided = 256;

  typedef itk::ImageFileReader<ImageType> ImageReaderType;

  typedef itk::LabelVotingImageFilter< ImageType > LabelVotingImageFilter;
  LabelVotingImageFilter::Pointer filter = LabelVotingImageFilter::New();
  filter->SetLabelForUndecidedPixels(labelforundecided);

  for(int i = 1; i < argc; i++){

    ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( argv[i] );
    reader->Update();
    filter->SetInput(i-1, reader->GetOutput());

  }
  
  filter->Update();

  string filename = "";
  char buffer[] = "/tmp/SPINEXXXXXXX";
  mktemp(buffer);
  filename = string(buffer);
  filename.append(".nii.gz");

  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(filename);
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  cout<<"{"<<endl;
  cout<<std::fixed;
  cout<<"\"filename\" : \""<<filename<<"\""<<endl;
  cout<<"}"<<endl;

  return EXIT_SUCCESS;

}