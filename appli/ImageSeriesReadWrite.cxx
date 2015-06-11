/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//  Software Guide : BeginLatex
//
//  This example illustrates how to read a series of 2D slices from independent
//  files in order to compose a volume. The class \doxygen{ImageSeriesReader}
//  is used for this purpose. This class works in combination with a generator
//  of filenames that will provide a list of files to be read. In this
//  particular example we use the \doxygen{NumericSeriesFileNames} class as
//  filename generator. This generator uses a \code{printf} style of string format
//  with a ``\code{\%d}'' field that will be successively replaced by a number specified
//  by the user. Here we will use a format like ``\code{file\%03d.png}'' for reading
//  PNG files named file001.png, file002.png, file003.png... and so on.
//
//  This requires the following headers as shown.
//
//  \index{itk::ImageSeriesReader!header}
//  \index{itk::NumericSeriesFileNames!header}
//
//  Software Guide : EndLatex

#include "itkOrientImageFilter.h"
// Software Guide : BeginCodeSnippet
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkGE5ImageIO.h"
#include "itkGE4ImageIO.h"
#include "itkIPLCommonImageIO.h"
#include "itkRawImageIO.h"

#include "itkImageRegionIterator.h"

#include "string"

// Software Guide : EndCodeSnippet

using namespace std;

void help(char* exec){
    cerr<<"Usage: "<<exec<<" -f <First file of Genesis stack> ex: "<<exec<<" -f I.001"<<endl;
    cerr<<"Options: "<<endl;
    cerr<<"-ie <Input filename. Example image. The output image will have the same characteristics, spacing, origin etc.>"<<endl;
    cerr<<"-o <Output filename>"<<endl;
}

template<typename ImageType>
typename ImageType::Pointer getImage2D(string filename){

    typedef typename ImageType::Pointer ImageTypePointer;
    typedef itk::ImageSeriesReader< ImageType >  ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();

    reader->SetImageIO( itk::GE5ImageIO::New() );
    reader->SetFileName(filename);
    try{
        reader->Update();
        ImageTypePointer img = reader->GetOutput();
        return img;
    }catch(itk::ExceptionObject &e){
        cerr<<e<<endl;
        return NULL;
    }
}

template<typename ImageType>
vector<typename ImageType::Pointer> getAll2DImages(vector<string> filenames){

    typedef typename ImageType::Pointer ImageTypePointer;
    vector<ImageTypePointer> allimages;

    for(int i = 0; i < filenames.size(); i++){

        ImageTypePointer p = getImage2D<ImageType>(filenames[i]);
        if(p){
            allimages.push_back(p);
        }
    }
    return allimages;
}

template<typename ImageType>
vector<string> getAll2DFileNames(vector<string> filenames){

    vector<string> filenamesActive;
    typedef itk::ImageSeriesReader< ImageType >  ReaderType;
    for(int i = 0; i < filenames.size(); i++){
        typename ReaderType::Pointer reader = ReaderType::New();

        reader->SetImageIO( itk::GE5ImageIO::New() );
        reader->SetFileName(filenames[i]);
        try{
            reader->Update();
            filenamesActive.push_back(filenames[i]);
        }catch(itk::ExceptionObject &e){
            cerr<<e<<endl;
        }
    }
    return filenamesActive;
}

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main( int argc, char ** argv )
{


  // Software Guide : BeginLatex
  //
  // We start by defining the \code{PixelType} and \code{ImageType}.
  //
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef unsigned short                       PixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageSeriesReader< ImageType >  ReaderType;
  typedef itk::ImageFileWriter<   ImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  // Software Guide : EndCodeSnippet


  string firstSliceName = "";
  string outputFileName = "out.nii.gz";
  string inputFileExample = "";

  for(int i = 1; i < argc - 1; i++){
      
      if(string(argv[i]).compare("-f") == 0){
          firstSliceName = string(argv[i+1]);
      }else if(string(argv[i]).compare("-o") == 0){
          outputFileName = string(argv[i+1]);
      }else if(string(argv[i]).compare("-ie") == 0){
          inputFileExample = string(argv[i+1]);
      }
  }

  if(firstSliceName == ""){
      help(argv[0]);
      return 0;
  }

  ImageType::Pointer inputExample = 0;
  if(inputFileExample.compare("") != 0){
      typedef itk::ImageFileReader<ImageType> ImageReaderType;
      ImageReaderType::Pointer read = ImageReaderType::New();
      read->SetFileName(inputFileExample);
      read->Update();
      inputExample = read->GetOutput();
  }

  typedef itk::NumericSeriesFileNames    NameGeneratorType;

  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  nameGenerator->SetSeriesFormat( firstSliceName.substr(0, firstSliceName.find_last_of(".")) + ".%03d" );
  nameGenerator->SetStartIndex( 1 );
  nameGenerator->SetEndIndex( 1 );
  nameGenerator->SetIncrementIndex( 1 );
  vector<string> filenames = nameGenerator->GetFileNames();
  reader->SetFileNames( filenames );

  reader->SetImageIO( itk::GE5ImageIO::New() );

  ImageType::Pointer resimage = 0;
  try{
    reader->Update();
    resimage = reader->GetOutput();
  }catch( itk::ExceptionObject & err ){
      cerr<< err <<endl;
      cerr<<"Trying slice by slice..."<<endl;
      try{
          NameGeneratorType::Pointer nameGenerator2D = NameGeneratorType::New();
          nameGenerator2D->SetSeriesFormat( firstSliceName.substr(0, firstSliceName.find_last_of(".")) + ".%03d" );
          nameGenerator2D->SetStartIndex( 1 );
          nameGenerator2D->SetEndIndex( 56 );
          nameGenerator2D->SetIncrementIndex( 1 );
          vector<string> filenames = nameGenerator2D->GetFileNames();

          typedef itk::Image< PixelType, 2 >  ImageType2D;
          filenames = getAll2DFileNames<ImageType2D>(filenames);

          if(filenames.size() == 0){
              throw itk::ExceptionObject("ImageSeriesReadWrite", 0, "No slices could be read.");
          }else{
              cout<<"Slices read: "<<filenames.size()<<endl;

              ImageType2D::Pointer img2d = getImage2D<ImageType2D>(firstSliceName);

              ImageType::SizeType size;
              ImageType2D::SizeType img2dsize = img2d->GetLargestPossibleRegion().GetSize();
              size[0] = img2dsize[0];
              size[1] = img2dsize[0];
              size[2] = filenames.size();
              double spc[3];
              spc[0] = img2d->GetSpacing()[0];
              spc[1] = img2d->GetSpacing()[1];
              spc[2] = 3;

              typedef itk::RawImageIO<PixelType, 2> RawIOType;
              RawIOType::Pointer rawio = RawIOType::New();
              rawio->SetDimensions( 0, size[0] );
              rawio->SetDimensions( 1, size[1] );
              rawio->SetSpacing( 0, spc[0] );
              rawio->SetSpacing( 1, spc[1] );

              int s = filesize(firstSliceName.c_str());
              s -= size[0]*size[0]*sizeof(PixelType);
              if(s > 0){
                  rawio->SetHeaderSize(s);
              }else{
                  throw itk::ExceptionObject("ImageSeriesReadWrite", 0, "There is no header in this file");
              }

              reader->SetFileNames(filenames);
              reader->SetImageIO(rawio);
              reader->Update();
              resimage = reader->GetOutput();
              resimage->SetSpacing(spc);
          }


      }catch(itk::ExceptionObject & err ){
          cerr<< err <<endl;
          cerr << "Trying reader GE4..."<<endl;
          try{
              reader->SetImageIO(itk::GE4ImageIO::New());
              reader->Update();
              resimage = reader->GetOutput();
          }catch(itk::ExceptionObject & err){
              cerr<< err <<endl;
              cerr<<"Trying raw image reader..."<<endl;

              double spc[3];
              spc[0] = 0.9375;
              spc[1] = 0.9375;
              spc[2] = 3;
              double origin[3];
              origin[0] = 0;
              origin[1] = 0;
              origin[2] = 0;
              int dimensions[3];
              dimensions[0] = 256;
              dimensions[1] = 256;
              dimensions[2] = 54;

              ImageType::DirectionType dir;
              dir.SetIdentity();

              if(inputExample){
                  inputExample->GetOrigin();
                  ImageType::SpacingType tempspc = inputExample->GetSpacing();
                  spc[0] = tempspc[0];
                  spc[1] = tempspc[1];
                  spc[2] = tempspc[2];
                  ImageType::RegionType::SizeType size = inputExample->GetLargestPossibleRegion().GetSize();
                  dimensions[0] = size[0];
                  dimensions[1] = size[1];
                  dimensions[2] = size[2];
                  dir = inputExample->GetDirection();
              }

              nameGenerator->SetEndIndex( dimensions[2] );

              vector<string> filenames = nameGenerator->GetFileNames();

              reader->SetFileNames( filenames );

              typedef itk::RawImageIO<PixelType, 2> RawIOType;
              RawIOType::Pointer rawio = RawIOType::New();
              rawio->SetDimensions( 0, dimensions[0] );
              rawio->SetDimensions( 1, dimensions[1] );
              rawio->SetSpacing( 0, spc[0] );
              rawio->SetSpacing( 1, spc[1] );
              rawio->SetHeaderSize(0);
              reader->SetImageIO(rawio);

              try{
                  reader->Update();
                  resimage = reader->GetOutput();
                  resimage->SetSpacing(spc);
                  resimage->SetDirection(dir);
              }catch(itk::ExceptionObject & err ){
                  cerr << "ERROR: Complete failure to read this image. "<<endl;
                  cerr<<err<<endl;
                  return EXIT_FAILURE;
              }
          }

      }
  }

  cout<<endl<<"{\"outputFilename\" : \""<<outputFileName<<"\"}"<<endl;
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( resimage );
  try
    {
    writer->Update();
  }catch( itk::ExceptionObject & err )
    {
    cerr<< err << endl;
    return EXIT_FAILURE;
    }
  // Software Guide : EndCodeSnippet




  return EXIT_SUCCESS;
}
