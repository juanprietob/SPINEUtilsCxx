#include "spinecontourswriter.h"


#include "vtkObjectFactory.h"
vtkStandardNewMacro(SPINEContoursWriter);

SPINEContoursWriter::SPINEContoursWriter()
{
    FileName = 0;
}

SPINEContoursWriter::~SPINEContoursWriter()
{
}

void SPINEContoursWriter::Write(){

    vtkCollectionSimpleIterator it;
    InputData->InitTraversal(it);

    // Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException& toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << pMsg;
        XMLString::release(&pMsg);

    }


    int errorCode = 0;

    DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL)
    {
       try
       {
           DOMDocument* doc = impl->createDocument(
                       0,                    // root element namespace URI.
                       X("contours"),         // root element name
                       0);                   // document type object (DTD).

           DOMElement* rootElem = doc->getDocumentElement();

           rootElem->setAttribute(X("type"), X("boxplots"));

           DOMElement*  contours = doc->createElement(X("contours"));
           rootElem->appendChild(contours);

           for(int i = 0; i < InputData->GetNumberOfItems(); i++){
               vtkPolyData* nextpoly = InputData->GetNextPolyData(it);

               DOMElement*  contour = doc->createElement(X("contour"));
               rootElem->appendChild(contour);

               for(unsigned j = 0; j < nextpoly->GetNumberOfPoints(); j++){

                   double p[3];
                   nextpoly->GetPoint(j, p);

                   DOMElement*  point = doc->createElement(X("point"));
                   contour->appendChild(point);

                   char buf[50];
                   sprintf(buf, "%f", p[0]);
                   point->setAttribute(X("px"), X(buf));

                   sprintf(buf, "%f", p[1]);
                   point->setAttribute(X("py"), X(buf));

                   sprintf(buf, "%f", p[2]);
                   point->setAttribute(X("pz"), X(buf));


               }
           }

           this->OutputXML(doc, this->GetFileName());

           doc->release();

       }
       catch (const OutOfMemoryException&)
       {
           XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
           errorCode = 5;
       }
       catch (const DOMException& e)
       {
           XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
           errorCode = 2;
       }
       catch (...)
       {
           XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
           errorCode = 3;
       }
    }  // (inpl != NULL)
    else
    {
       XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
       errorCode = 4;
    }


   XMLPlatformUtils::Terminate();



}

void SPINEContoursWriter::OutputXML(xercesc::DOMDocument* pmyDOMDocument, const char* filePath)
{
    //Return the first registered implementation that has the desired features. In this case, we are after a DOM implementation that has the LS feature... or Load/Save.
    XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation *implementation = DOMImplementationRegistry::getDOMImplementation(tempStr);

    // Create a DOMLSSerializer which is used to serialize a DOM tree into an XML document.
    DOMLSSerializer *serializer = ((DOMImplementationLS*)implementation)->createLSSerializer();



    // Make the output more human readable by inserting line feeds.
    //if (serializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
    //    serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    // The end-of-line sequence of characters to be used in the XML being written out.
    //serializer->setNewLine(XMLString::transcode("\r\n"));

    // Convert the path into Xerces compatible XMLCh*.

    if(filePath){
        XMLCh *tempFilePath = XMLString::transcode(filePath);

        // Specify the target for the XML output.
        XMLFormatTarget *formatTarget = new LocalFileFormatTarget(tempFilePath);

        // Create a new empty output destination object.
        DOMLSOutput *output = ((DOMImplementationLS*)implementation)->createLSOutput();

        // Set the stream to our target.
        output->setByteStream(formatTarget);

        // Write the serialized output to the destination.
        serializer->write(pmyDOMDocument, output);

        XMLString::release(&tempFilePath);
        delete formatTarget;
        output->release();
    }else{

        string strdoc = XMLString::transcode(serializer->writeToString(pmyDOMDocument));
        cout<<strdoc<<endl<<endl;
    }

    // Cleanup.
    serializer->release();

}


void SPINEContoursWriter::PrintSelf(ostream &os, vtkIndent indent){

}
