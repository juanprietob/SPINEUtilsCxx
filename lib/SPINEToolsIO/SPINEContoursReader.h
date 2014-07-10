

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

using namespace std;

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
};
