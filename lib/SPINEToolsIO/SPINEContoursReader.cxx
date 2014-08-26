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
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

using namespace xercesc;

vtkStandardNewMacro(SPINEContoursReader);

//----------------------------------------------------------------------------
SPINEContoursReader::SPINEContoursReader()
{
  m_Output = vtkPolyDataCollection::New();
  this->FileContent = "";

  _bplotnames = vtkSmartPointer<vtkStringArray>::New();
  _bplotnames->SetName("boxplotsname");
  _bplotnames->InsertValue(0, "maxBd");
  _bplotnames->InsertValue(1, "sup");
  _bplotnames->InsertValue(2, "med");
  _bplotnames->InsertValue(3, "inf");
  _bplotnames->InsertValue(4, "minBd");
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

  vtkDebugMacro(<<"Reading contours SPINE data...");

  try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cerr << "Error during initialization! :\n"
             << message << "\n";
        XMLString::release(&message);
        return 1;
    }

    XercesDOMParser* parser = new XercesDOMParser();
    //parser->setValidationScheme(XercesDOMParser::Val_Always);
   //parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    try {


        if(this->GetFileName()){
            vtkDebugMacro(<<"Filename= "<<this->GetFileName());
            parser->parse(this->GetFileName());
        }else if(this->FileContent.compare("") != 0){
            vtkDebugMacro(<<"Parsing string= "<<this->FileContent);
            xercesc::MemBufInputSource myxml_buf((const XMLByte*)this->FileContent.c_str(), this->FileContent.size(), "FileContent", false);
            parser->parse(myxml_buf);
        }


        DOMNodeList* contours = parser->getDocument()->getElementsByTagName(XMLString::transcode("contour"));
        XMLSize_t size = contours->getLength();

        for(XMLSize_t i = 0; i < size; i++){
            DOMNode* contour = contours->item(i);

            DOMNodeList* contourNodes = contour->getChildNodes();
            vtkSmartPointer<vtkPolyData> vtkcontour = vtkSmartPointer<vtkPolyData>::New();

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


                }

                if(XMLString::compareString(contourNodes->item(j)->getNodeName(), XMLString::transcode("area")) == 0){
                    DOMNodeList* areasnode = parser->getDocument()->getElementsByTagName(XMLString::transcode("area"));
                    if(areasnode){

                        vtkSmartPointer<vtkDoubleArray> bplotareas = vtkSmartPointer<vtkDoubleArray>::New();
                        bplotareas->SetName("boxplotsarea");

                        XMLSize_t size = areasnode->getLength();

                        for(XMLSize_t i = 0; i < size; i++){
                            DOMNode* areanode = areasnode->item(i);
                            for(unsigned j = 0; j < _bplotnames->GetNumberOfValues(); j++){
                                string name = _bplotnames->GetValue(j);
                                double area = atof(XMLString::transcode(areanode->getAttributes()->getNamedItem(X(name.c_str()))->getNodeValue()));
                                bplotareas->InsertNextValue(area);
                                //cout<<name<<" "<< area<<endl;


                            }

                        }

                        vtkcontour->GetPointData()->AddArray(_bplotnames);
                        vtkcontour->GetPointData()->AddArray(bplotareas);
                    }

                }
            }

            m_Output->AddItem(vtkcontour);
        }


    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cerr << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cerr << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        cerr << "Unexpected Exception \n" ;
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
