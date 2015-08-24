

#include <vtkDataReader.h>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <iostream>

#include <vtkPolyData.h>
#include <vtkPolyDataCollection.h>
#include <vtkSmartPointer.h>
#include <vtkLine.h>
#include "vtkStringArray.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

#define X(str) XStr(str).unicodeForm()

class VTKIOLEGACY_EXPORT SPINEContoursReader : public vtkDataReader
{
public:

	static SPINEContoursReader *New();
    vtkTypeMacro(SPINEContoursReader,vtkDataReader)
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Get the output of this reader.
    vtkPolyDataCollection *GetOutput();
    void SetOutput(vtkPolyDataCollection *output);

    vtkSetMacro(FileContent, string)
    vtkGetMacro(FileContent, string)

protected:

	SPINEContoursReader();
	~SPINEContoursReader();

	int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *);

	int FillOutputPortInformation(int, vtkInformation*);

private:

  SPINEContoursReader(const SPINEContoursReader&);  // Not implemented.
  void operator=(const SPINEContoursReader&);  // Not implemented.

  vtkPolyDataCollection* m_Output;
  string FileContent;
  vtkSmartPointer<vtkStringArray> _bplotnames;
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
