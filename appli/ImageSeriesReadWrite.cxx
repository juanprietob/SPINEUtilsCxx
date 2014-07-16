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

#include "string"

// Software Guide : EndCodeSnippet

using namespace std;

void help(char* exec){
    cout<<"Usage: "<<exec<<" -f <First file of Genesis stack> ex: "<<exec<<" -f <patient path>/T2/I.001"<<endl;
    cout<<"Output: The output filename is calculated from the directory. ex: <patient path>/T2.nii.gz"<<endl;
    cout<<"Options: "<<endl;
    cout<<"-it <input format> ex -it dicom"<<endl;
    cout<<"-o <Output filename>"<<endl;
    cout<<"\n\t SPINE related SPINEUtils: -r <root dir>, used for DB insertion in table ModuleData. The last directory of <root dir> is used as the name identifier."<<endl;
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
  typedef short                       PixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  // Software Guide : EndCodeSnippet


  // Software Guide : BeginLatex
  //
  // The image type is used as a template parameter to instantiate
  // the reader and writer.
  //
  // \index{itk::ImageSeriesReader!Instantiation}
  // \index{itk::ImageFileWriter!Instantiation}
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::ImageSeriesReader< ImageType >  ReaderType;
  typedef itk::ImageFileWriter<   ImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  // Software Guide : EndCodeSnippet


  string firstSliceName = "";
  string rootDir = "";
  string outputFileName = "";


  for(int i = 1; i < argc - 1; i++){
      
      if(string(argv[i]).compare("-f") == 0){
          firstSliceName = string(argv[i+1]);
      }else if(string(argv[i]).compare("-d") == 0){
          rootDir = string(argv[i+1]);
      }else if(string(argv[i]).compare("-o") == 0){
          outputFileName = string(argv[i+1]);
      }
  }

  cout<<"firstSliceName= " + firstSliceName<<endl;

  if(firstSliceName == ""){
      help(argv[0]);
      return 0;
  }

  if(rootDir != ""){
      rootDir = rootDir.substr(0, rootDir.find_last_of("/"));
      rootDir = rootDir.substr(rootDir.find_last_of("/") + 1);
  }

  string imagetype = "";
  if(outputFileName == ""){
      outputFileName = firstSliceName.substr(0, firstSliceName.find_last_of("/"));

      imagetype = outputFileName.substr(outputFileName.find_last_of("/") + 1);
      string outImageName = imagetype + ".nii.gz";

      outputFileName = outputFileName.substr(0, outputFileName.find_last_of("/") + 1) + outImageName;
  }


  // Software Guide : BeginLatex
  //
  // Then, we declare the filenames generator type and create one instance of it.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::NumericSeriesFileNames    NameGeneratorType;

  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  // Software Guide : EndCodeSnippet



  // Software Guide : BeginLatex
  //
  // The filenames generator requires us to provide a pattern of text for the
  // filenames, and numbers for the initial value, last value and increment to be
  // used for generating the names of the files.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  nameGenerator->SetSeriesFormat( firstSliceName.substr(0, firstSliceName.find_last_of(".")) + ".%03d" );

  nameGenerator->SetStartIndex( 1 );
  nameGenerator->SetEndIndex( 1 );
  nameGenerator->SetIncrementIndex( 1 );
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  The ImageIO object that actually performs the read process is now connected
  //  to the ImageSeriesReader. This is the safest way of making sure that we use
  //  an ImageIO object that is appropriate for the type of files that we want to
  //  read.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  reader->SetFileNames( nameGenerator->GetFileNames()  );


  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  The filenames of the input files must be provided to the reader. While the
  //  writer is instructed to write the same volume dataset in a single file.
  //
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet

  reader->SetImageIO( itk::GE5ImageIO::New() );

  ImageType::Pointer resimage = 0;
  try{
    resimage = reader->GetOutput();
  }catch( itk::ExceptionObject & err ){
      cout << "ERROR: Trying reader GE4"<<endl;

      reader->SetImageIO(itk::GE4ImageIO::New());
      try{
          resimage = reader->GetOutput();
      }catch(itk::ExceptionObject & err ){
          cout << "ERROR: " <<firstSliceName<<endl;
          cout<< err << endl;
          return EXIT_FAILURE;
      }
  }

  cout<<endl<<"Writing to: "<<outputFileName<<endl;
  writer->SetFileName( outputFileName.c_str() );
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
  writer->SetInput( resimage );
  // Software Guide : EndCodeSnippet



  //  Software Guide : BeginLatex
  //
  //  Finally, execution of the pipeline can be triggered by invoking the
  //  Update() method in the writer. This call must be placed in a try/catch
  //  block since exceptions be potentially be thrown in the process of reading
  //  or writing the images.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  try
    {
    writer->Update();
  }catch( itk::ExceptionObject & err )
    {
    cout << "ERROR: " <<rootDir<<"/"<<imagetype<<endl;
    cout<< err << endl;
    return EXIT_FAILURE;
    }
  // Software Guide : EndCodeSnippet




  return EXIT_SUCCESS;
}
