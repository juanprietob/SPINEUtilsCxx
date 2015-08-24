#ifndef CONTOURTOIMAGEFILTER_H
#define CONTOURTOIMAGEFILTER_H


#include "vtkImageAlgorithm.h"

class ContourToImageFilter : public vtkImageAlgorithm
{
public:
    static ContourToImageFilter *New();
    vtkTypeMacro(ContourToImageFilter,vtkImageAlgorithm)
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetMacro(Vector, const double*)
    vtkSetMacro(Spacing, const double*)
    vtkSetMacro(Origin, const double*)
    vtkSetMacro(Dimensions, const int*)
protected:
    ContourToImageFilter();


    virtual int FillInputPortInformation(int, vtkInformation *info);
     virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *outputVector);

private:
    const double* Vector;
    const double* Spacing;
    const double* Origin;
    const int* Dimensions;
};

#endif // CONTOURTOIMAGEFILTER_H
