


#ifndef __SPINEContoursInterpolation_h
#define __SPINEContoursInterpolation_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
#include "vtkSmartPointer.h"



class VTKFILTERSCORE_EXPORT SPINEContoursInterpolation : public vtkPolyDataAlgorithm
{
public:
  static SPINEContoursInterpolation *New();
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeMacro(SPINEContoursInterpolation,vtkPolyDataAlgorithm);

    vtkSetMacro(MaximumCurveError, double)
    vtkGetMacro(MaximumCurveError, double)

    vtkSetMacro(ContourLength, double)
    vtkGetMacro(ContourLength, double)

    vtkSetMacro(NumberOfPoints, int)
    vtkGetMacro(NumberOfPoints, int)


    vtkGetMacro(AvgNormal, double*)

    vtkGetMacro(Area, double)
protected:
  SPINEContoursInterpolation();
 ~SPINEContoursInterpolation();

  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  SPINEContoursInterpolation(const SPINEContoursInterpolation&);  // Not implemented.
  void operator=(const SPINEContoursInterpolation&);  // Not implemented.

  void getDerivative(int i, vtkPoints* points, double* dp0);
  void getInterpolatedPoint(const double* p1, const double* p2, const double* p3, const double* p4, double* p, double t);

  double MaximumCurveError;
  double ContourLength;
  int NumberOfPoints;
  double* AvgNormal;
  double Area;
};

        #endif

