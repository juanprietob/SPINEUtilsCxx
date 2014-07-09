/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SPINEContoursReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "SPINEContoursReader.h"

#include "vtkCellArray.h"
#include "vtkFieldData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

using namespace xercesc;

vtkStandardNewMacro(SPINEContoursReader);

//----------------------------------------------------------------------------
SPINEContoursReader::SPINEContoursReader()
{
  m_Output = vtkPolyDataCollection::New();

}

//----------------------------------------------------------------------------
SPINEContoursReader::~SPINEContoursReader()
{
}

//----------------------------------------------------------------------------
vtkPolyDataCollection *SPINEContoursReader::GetOutput()
{
  return m_Output;
}

//----------------------------------------------------------------------------
int SPINEContoursReader::RequestUpdateExtent(
  vtkInformation *,
  vtkInformationVector **,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  int piece, numPieces, ghostLevel;

  piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  numPieces = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
  ghostLevel = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());

  // make sure piece is valid
  if (piece < 0 || piece >= numPieces)
    {
    return 1;
    }

  if (ghostLevel < 0)
    {
    return 1;
    }

  return 1;
}

//----------------------------------------------------------------------------
int SPINEContoursReader::RequestData(
  vtkInformation *,
  vtkInformationVector **,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  int numPts=0;
  char line[256];
  int npts, size = 0, ncells, i;
  int done=0;
  vtkCollection *output = vtkCollection::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  int *tempArray;
  vtkIdType *idArray;

  vtkDebugMacro(<<"Reading vtk polygonal data...");

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


      vtkDebugMacro(<<"Filename= "<<this->GetFileName());

        parser->parse(this->GetFileName());


        DOMNodeList* contours = parser->getDocument()->getElementsByTagName(XMLString::transcode("contour"));
        XMLSize_t size = contours->getLength();

        for(XMLSize_t i = 0; i < size; i++){
            DOMNode* contour = contours->item(i);

            DOMNodeList* contourNodes = contour->getChildNodes();
            for(XMLSize_t j = 0; j < contourNodes->getLength(); j++){



                if(XMLString::compareString(contourNodes->item(j)->getNodeName(), XMLString::transcode("points")) == 0){

                    DOMNodeList* pointList = contourNodes->item(j)->getChildNodes();
                    vtkSmartPointer<vtkPoints> vtkpoints = vtkSmartPointer<vtkPoints>::New();

                    for(XMLSize_t k = 0; k < pointList->getLength(); k++){

                        if(XMLString::compareString(pointList->item(k)->getNodeName(), XMLString::transcode("point")) == 0){
                            //vtkDebugMacro(<<point);
                            DOMNode* point = pointList->item(k);
                            double x = atof(XMLString::transcode(point->getAttributes()->getNamedItem(XMLString::transcode("px"))->getTextContent()));
                            double y = atof(XMLString::transcode(point->getAttributes()->getNamedItem(XMLString::transcode("py"))->getTextContent()));
                            double z = atof(XMLString::transcode(point->getAttributes()->getNamedItem(XMLString::transcode("pz"))->getTextContent()));

                            vtkpoints->InsertNextPoint(x, y, z);
                        }
                    }

                    vtkSmartPointer<vtkPolyData> vtkcontour = vtkSmartPointer<vtkPolyData>::New();
                    vtkSmartPointer<vtkCellArray> vtkcellarray = vtkSmartPointer<vtkCellArray>::New();

                    for(unsigned k = 0; k < vtkpoints->GetNumberOfPoints(); k++){

                        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
                        line->GetPointIds()->SetId(0, k);
                        if(k == vtkpoints->GetNumberOfPoints() - 1){
                            line->GetPointIds()->SetId(1, 0);
                        }else{
                            line->GetPointIds()->SetId(1, k + 1);
                        }

                        vtkcellarray->InsertNextCell(line);
                    }

                    vtkcontour->SetPoints(vtkpoints);
                    vtkcontour->SetLines(vtkcellarray);

                    //cout<<vtkcontour;

                    m_Output->AddItem(vtkcontour);
                }
            }
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

  return 1;
}

//----------------------------------------------------------------------------
int SPINEContoursReader::FillOutputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}

//----------------------------------------------------------------------------
void SPINEContoursReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
