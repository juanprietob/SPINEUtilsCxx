
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkImageFileWriter.h"

#include <iostream>
#include <string>

using namespace std;

void help(char* exec){
    cerr<<"Extract a slice from a volume. The image can be set through std input or with filename"<<endl;
    cerr<<"ex. cat <imageFilename> | "<<string(exec)<<" -x <num>"<<endl;
    cerr<<"options: "<<endl;
    cerr<<"-f <filename>"<<endl;
    cerr<<"-x <int>"<<endl;
    cerr<<"-y <int>"<<endl;
    cerr<<"-z <int>"<<endl;
    cerr<<"-out <outfilename>"<<endl;
}

int main(int argc, char *argv[])
{
    string filename = "";
    string outfilename = "";
    bool slicexyz[3];
    double slicenum;

    slicexyz[0] = false;
    slicexyz[1] = false;
    slicexyz[2] = false;

    for(int i = 1; i < argc; i++){
        string param = string(argv[i]);
        if(param.compare("-f") == 0){
            filename = string(argv[i+1]);
        }else if(param.compare("-x") == 0){
            slicenum = atof(argv[i+1]);
            slicexyz[0] = true;
        }else if(param.compare("-y") == 0){
            slicenum = atof(argv[i+1]);
            slicexyz[1] = true;
        }else if(param.compare("-z") == 0){
            slicenum = atof(argv[i+1]);
            slicexyz[2] = true;
        }else if(param.compare("-out") == 0){
            outfilename = string(argv[i+1]);
        }else if(param.compare("-h") == 0 || param.compare("-help") == 0){
            help(argv[0]);
            return 0;
        }
    }

    if(!slicexyz[0] && !slicexyz[1] && !slicexyz[2]){
        help(argv[0]);
        return 0;
    }


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


  typedef unsigned short PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::ImageFileReader<ImageType>             ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.c_str());
  reader->Update();

  ImageType::Pointer img = reader->GetOutput();

  typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();

  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  ImageType::SizeType size = img->GetLargestPossibleRegion().GetSize();

  for(int i = 0; i < 3; i++){
      if(slicexyz[i]){
          start[i] = slicenum;
          size[i] = 1;
      }
  }

  ImageType::RegionType desiredRegion;
  desiredRegion.SetSize(size);
  desiredRegion.SetIndex(start);

  filter->SetRegionOfInterest(desiredRegion);
  filter->SetInput(img);
  filter->Update();
  ImageType::Pointer outimg = filter->GetOutput();

  if(outfilename.compare("") == 0){
      if(isTempFile){
          outfilename = filename;
      }else{
          outfilename = "out.nii.gz";
      }
  }

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outfilename.c_str());
  writer->SetInput(outimg);
  writer->Update();

  if(isTempFile){
      ifstream tempInFile(outfilename.c_str());
      char c;
      while(tempInFile.get(c)){
          cout<<c;
      }
      tempInFile.close();
      remove(filename.c_str());
  }

  return EXIT_SUCCESS;
}
