

#include <vtkDataReader.h>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>

#include <vtkPolyData.h>
#include <vtkPolyDataCollection.h>
#include <vtkSmartPointer.h>
#include <vtkLine.h>


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

protected:

	SPINEContoursReader();
	~SPINEContoursReader();

	int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *);

	// Update extent of PolyData is specified in pieces.
	// Since all DataObjects should be able to set UpdateExent as pieces,
	// just copy output->UpdateExtent  all Inputs.
	int RequestUpdateExtent(vtkInformation *, vtkInformationVector **,
	                  	vtkInformationVector *);

	int FillOutputPortInformation(int, vtkInformation*);

private:

  SPINEContoursReader(const SPINEContoursReader&);  // Not implemented.
  void operator=(const SPINEContoursReader&);  // Not implemented.

  vtkPolyDataCollection* m_Output;
};
