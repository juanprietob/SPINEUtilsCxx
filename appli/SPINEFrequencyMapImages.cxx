#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include <vector>

using namespace std;

void help(char* exec){
    cerr<<"Calculates frequency map from several images in fonction of indicated reference label."<<endl;
    cerr<<"How to use: "<<string(exec)<<" <Label> <Filename 1> <Filename 2> ... <Filename N>"<<endl;
}

int main( int argc, char ** argv )
{

  if(argc < 4){
      help(argv[0]);
      return 0;
  }

  int rate = (atoi)(argv[1]);
  int label = (atoi)(argv[2]);
  int labelForUndecided = 0;
  int nbImages = argc - 3;

  typedef unsigned short PixelType;
  static const int dimension = 3;
  typedef itk::Image< PixelType, dimension > ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;


  //Counter initialization
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( argv[3] );
  reader->Update();

  ImageType::Pointer image = reader->GetOutput();
  ImageType::RegionType region = image->GetRequestedRegion();

  ImageType::Pointer counter = ImageType::New();
  counter->SetDirection(image->GetDirection());
  counter->SetRegions(region);
  counter->Allocate();

  itk::ImageRegionIterator<ImageType> counterIterator(counter, counter->GetRequestedRegion());
  counterIterator.GoToBegin();
  while(!counterIterator.IsAtEnd())
  {
    counterIterator.Set(0);
    ++counterIterator;
  }

  //Process images
  for(int i = 3; i < argc; i++){

    ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( argv[i] );
    reader->Update();

    ImageType::Pointer image = reader->GetOutput();
    itk::ImageRegionIterator<ImageType> imageIterator(image, image->GetRequestedRegion());

    imageIterator.GoToBegin();
    counterIterator.GoToBegin();
    while(!imageIterator.IsAtEnd())
    {
      // Get the value of the current voxel
      int val = imageIterator.Get();

      if(val == label)
      {
        counterIterator.Set(counterIterator.Get()+1);
      }
   
      ++imageIterator;
      ++counterIterator;
    }

  }
  
  string filename = "";
  char buffer[] = "/tmp/SPINEXXXXXXX";
  mktemp(buffer);
  filename = string(buffer);
  filename.append(".nii.gz");

  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(filename);
  writer->SetInput( counter );
  writer->Update();

  cout<<"{"<<endl;
  cout<<std::fixed;
  cout<<"\"filename\" : \""<<filename<<"\""<<endl;
  cout<<"}"<<endl;

  return EXIT_SUCCESS;
}
