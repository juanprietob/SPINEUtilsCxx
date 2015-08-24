/*=========================================================================

  Program:   Slicer
  Language:  C++
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

} // end of anonymous namespace

using namespace std;
using namespace xercesc;

int main( int argc, char *argv[] )
{


    string inputVolume = argv[1];
    string outputVolume = argv[2];
    string seedFile = argv[3];
    int smoothingIterations = 0;
    double timestep = 0.0625;
    double multiplier = 1;
    int iterations = 1;
    int neighborhood = 1;
    int labelvalue = 2;
    vector< vector< double > > seed;


    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Error during initialization! :\n"
             << message << "\n";
        XMLString::release(&message);
        return 1;
    }

    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    try {
        parser->parse(seedFile.c_str());


        DOMNodeList* coordslist = parser->getDocument()->getElementsByTagName(XMLString::transcode("coords"));
        XMLSize_t size = coordslist->getLength();
        for(XMLSize_t i = 7; i < 8; i++){
            DOMNode* node = coordslist->item(i);
            int x = atoi(XMLString::transcode(node->getAttributes()->getNamedItem(XMLString::transcode("x"))->getTextContent()));
            int y = atoi(XMLString::transcode(node->getAttributes()->getNamedItem(XMLString::transcode("y"))->getTextContent()));
            int z = atoi(XMLString::transcode(node->getAttributes()->getNamedItem(XMLString::transcode("z"))->getTextContent()));

            vector<double> coord;
            coord.push_back(x);
            coord.push_back(y);
            coord.push_back(z);

            seed.push_back(coord);


        }
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        cout << "Unexpected Exception \n" ;
        return -1;
    }

    delete parser;
    delete errHandler;

  typedef   float InternalPixelType;
  const     unsigned int Dimension = 3;
  typedef itk::Image<InternalPixelType, Dimension> InternalImageType;

  typedef unsigned short                         OutputPixelType;
  typedef itk::Image<OutputPixelType, Dimension> OutputImageType;

  typedef itk::CastImageFilter<InternalImageType, OutputImageType>
  CastingFilterType;
  CastingFilterType::Pointer caster = CastingFilterType::New();

  typedef  itk::ImageFileReader<InternalImageType> ReaderType;
  typedef  itk::ImageFileWriter<OutputImageType>   WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( inputVolume.c_str() );
  reader->Update();

  writer->SetFileName( outputVolume.c_str() );

  typedef itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType>
  CurvatureFlowImageFilterType;
  CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();

  typedef itk::ConfidenceConnectedImageFilter<InternalImageType, InternalImageType>
  ConnectedFilterType;
  ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();

  smoothing->SetInput( reader->GetOutput() );
  confidenceConnected->SetInput( smoothing->GetOutput() );
  caster->SetInput( confidenceConnected->GetOutput() );
  writer->SetInput( caster->GetOutput() );
  writer->SetUseCompression(1);

  smoothing->SetNumberOfIterations( smoothingIterations );
  smoothing->SetTimeStep( timestep );

  confidenceConnected->SetMultiplier( multiplier );
  confidenceConnected->SetNumberOfIterations( iterations );
  confidenceConnected->SetReplaceValue( labelvalue );
  confidenceConnected->SetInitialNeighborhoodRadius( neighborhood );

  if( seed.size() > 0 )
    {
    InternalImageType::PointType lpsPoint;
    InternalImageType::IndexType index;
    for( ::size_t i = 0; i < seed.size(); ++i )
      {

      index[0] = seed[i][0];
      index[1] = seed[i][1];
      index[2] = seed[i][2];

      //reader->GetOutput()->TransformPhysicalPointToIndex(lpsPoint, index);

      confidenceConnected->AddSeed(index);

//       std::cout << "LPS: " << lpsPoint << std::endl;
//       std::cout << "IJK: " << index << std::endl;
      }
    }
  else
    {
    std::cerr << "No seeds specified." << std::endl;
    return -1;
    }

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  return 0;
}
