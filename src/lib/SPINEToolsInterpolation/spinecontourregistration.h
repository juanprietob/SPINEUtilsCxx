#ifndef SPINECONTOURREGISTRATION_H
#define SPINECONTOURREGISTRATION_H

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"

class SPINEContourRegistration : public vtkPolyDataAlgorithm
{
public:
    static SPINEContourRegistration *New();
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeMacro(SPINEContourRegistration,vtkPolyDataAlgorithm);

    vtkSetMacro(SimilarityTransform, bool)
    vtkGetMacro(SimilarityTransform, bool)

    //vtkSetObjectMacro(InputTarget, vtkPolyData)
    //vtkGetObjectMacro(InputTarget, vtkPolyData)
    void SetInputTarget(vtkPolyData* target){
        this->InputTarget = target;
    }

  protected:
    SPINEContourRegistration();
   ~SPINEContourRegistration();

    // Usual data generation method
    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
    bool SimilarityTransform;
    vtkPolyData* InputTarget;
};

#endif // SPINECONTOURREGISTRATION_H
