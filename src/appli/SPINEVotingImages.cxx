#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include <vector>

using namespace std;

void help(char* exec){
    cerr<<"Calculates voting from several images in fonction of indicated rate and reference label."<<endl;
    cerr<<"How to use: "<<string(exec)<<" <Rate> <Label> <Filename 1> <Filename 2> ... <Filename N>"<<endl;
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
  counter->SetOrigin(image->GetOrigin());
  counter->Allocate();
  //counter->FillBuffer(0);
  itk::ImageRegionIterator<ImageType> counterIterator(counter, counter->GetRequestedRegion()); // TODO: Change for image->GetRequestedRegion() after http://www.itk.org/Doxygen/html/classitk_1_1ImageRegionIterator.html
  counterIterator.GoToBegin();
  while(!counterIterator.IsAtEnd())
  {
    counterIterator.Set(0);
    ++counterIterator;
  }

  //Create output image
  ImageType::Pointer output = ImageType::New();
  output->SetDirection(image->GetDirection());
  output->SetOrigin(image->GetOrigin());
  output->SetRegions(region);
  output->Allocate();
  //output->FillBuffer(0);
  itk::ImageRegionIterator<ImageType> outputIterator(output, output->GetRequestedRegion());

  //Process images
  for(int i = 3; i < argc; i++){

    ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( argv[i] );
    reader->Update();

    ImageType::Pointer image = reader->GetOutput();

    // ImageType::RegionType region = image->GetLargestPossibleRegion();
    // ImageType::SizeType regionSize = region.GetSize();
   
    // ImageType::IndexType regionIndex;
    // regionIndex[0] = 0;
    // regionIndex[1] = 0;
    // regionIndex[2] = 0;
    // region.SetIndex(regionIndex);

    //itk::ImageRegionIterator<ImageType> imageIterator(image,region);

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

  counterIterator.GoToBegin();
  outputIterator.GoToBegin();
  while(!counterIterator.IsAtEnd())
  {
    int val = counterIterator.Get();

    if((val/nbImages)*100 >= rate)
    {
      outputIterator.Set(label);
    } 
    else
    {
      outputIterator.Set(labelForUndecided);
    } 

    ++counterIterator;
    ++outputIterator;
  }
  
  string filename = "";
  char buffer[] = "/tmp/SPINEXXXXXXX";
  mktemp(buffer);
  filename = string(buffer);
  filename.append(".nii.gz");

  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(filename);
  writer->SetInput( output );
  writer->Update();

  cout<<"{"<<endl;
  cout<<std::fixed;
  cout<<"\"filename\" : \""<<filename<<"\""<<endl;
  cout<<"}"<<endl;

  return EXIT_SUCCESS;

}
