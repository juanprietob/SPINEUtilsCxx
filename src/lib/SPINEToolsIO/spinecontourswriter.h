#ifndef SPINECONTOURSWRITER_H
#define SPINECONTOURSWRITER_H

#include "vtkDataWriter.h"
#include "vtkPolyDataCollection.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>


#include <iostream>

XERCES_CPP_NAMESPACE_USE

#define X(str) XStr(str).unicodeForm()

using namespace std;

class SPINEContoursWriter : public vtkAlgorithm
{
public:


    static SPINEContoursWriter *New();
    vtkTypeMacro(SPINEContoursWriter,vtkAlgorithm)
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetObjectMacro(InputData, vtkPolyDataCollection)
    void Write();

    vtkSetMacro(FileName,const char*)
    vtkGetMacro(FileName,const char*)

    vtkSetMacro(ContoursType,const char*)
    vtkGetMacro(ContoursType,const char*)

protected:
    SPINEContoursWriter();
    ~SPINEContoursWriter();



private:

  SPINEContoursWriter(const SPINEContoursWriter&);  // Not implemented.
  void operator=(const SPINEContoursWriter&);  // Not implemented.

    vtkPolyDataCollection* InputData;
    const char* FileName;
    const char* ContoursType;

    void OutputXML(xercesc::DOMDocument *doc, const char *filename);
};

#ifndef XSTR_H
#define XSTR_H

class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;


};
#endif

#endif // SPINECONTOURSWRITER_H
