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
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

// Software Guide : EndCodeSnippet

using namespace std;

void help(char* exec){
    cout<<"Usage: "<<exec<<" -d <folder with dicom> -o <output filename> ex: "<<exec<<" -d <patient path>/T2/"<<endl;
    cout<<"Options: "<<endl;
    cout<<"-f <input filename> instead of a dicom image use another image file"<<endl;
}

int main( int argc, char ** argv )
{

  string dirName = "";
  string outputFileName = "";
  string filename = "";


  for(int i = 1; i < argc - 1; i++){
      string param = string(argv[i]);
      if(param == "-d"){
          dirName = string(argv[i+1]);
      }else if(param == "-o"){
          outputFileName = string(argv[i+1]);
      }else if(param == "-f"){
          filename = string(argv[i+1]);
      }
  }

  if((dirName == "" && filename == "") || outputFileName == ""){
      help(argv[0]);
      return 0;
  }


  const unsigned int InputDimension = 3;

    typedef unsigned short PixelType;

    typedef itk::Image< PixelType, InputDimension > ImageType;

    typedef itk::ImageSeriesReader< ImageType > ReaderType;
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
    typedef itk::ImageFileWriter< ImageType > ImageWriterType;
  ImageType::Pointer resimage = 0;

  if(dirName!=""){

      ////////////////////////////////////////////////
      // 1) Read the input series

        ImageIOType::Pointer gdcmIO = ImageIOType::New();
        InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
        inputNames->SetInputDirectory( dirName.c_str() );

        const ReaderType::FileNamesContainer &filenames = inputNames->GetInputFileNames();

        ReaderType::Pointer reader = ReaderType::New();

        reader->SetImageIO( gdcmIO );
        reader->SetFileNames( filenames );
        try
          {
          reader->Update();
          }
        catch (itk::ExceptionObject &excp)
          {
          std::cerr << "Exception thrown while reading the series" << std::endl;
          std::cerr << excp << std::endl;
          return EXIT_FAILURE;
          }


      try{
        resimage = reader->GetOutput();
      }catch( itk::ExceptionObject & err ){
          cout<< err << endl;
          return EXIT_FAILURE;
      }
  }else{
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
  }

  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // We connect the output of the reader to the input of the writer.
  //
  // Software Guide : EndLatex
  itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter = itk::OrientImageFilter<ImageType,ImageType>::New();

  orienter->UseImageDirectionOn();
  orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS);
  orienter->SetInput(resimage);
  orienter->Update();
  resimage = orienter->GetOutput();

  // Software Guide : BeginCodeSnippet

  cout<<endl<<"Writing to: "<<outputFileName<<endl;
  ImageWriterType::Pointer writer = ImageWriterType::New();

  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( resimage );
  writer->Update();
  // Software Guide : EndCodeSnippet


  return EXIT_SUCCESS;
}
