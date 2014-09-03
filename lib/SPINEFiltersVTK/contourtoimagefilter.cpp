#include "contourtoimagefilter.h"

#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkMath.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"
#include "vtkInformationStringVectorKey.h"

vtkStandardNewMacro(ContourToImageFilter);

ContourToImageFilter::ContourToImageFilter()
{
    Vector = 0;
    Spacing = 0;
    Origin = 0;
    Dimensions = 0;



}

//-----------------------------------------------------------------------------
int ContourToImageFilter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

int ContourToImageFilter::RequestData(vtkInformation * request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector){

     vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
       vtkInformation *outInfo = outputVector->GetInformationObject(0);

       vtkPolyData *inData = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
       vtkImageData *outData = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

       if((Vector == 0 || Spacing == 0 || Origin == 0 || Dimensions == 0)){
           cerr<<"Vector or Spacing or Origin or Dimensions not set."<<endl;
           return 0;
       }


       const double* spacing = Spacing;
       const double* origin = Origin;
       const int* dim = Dimensions;

       vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();

       whiteImage->SetSpacing(spacing[0], spacing[1], spacing[2]);
       whiteImage->SetDimensions(dim[0], dim[1], dim[2]);
       whiteImage->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);

       whiteImage->SetOrigin(origin[0], origin[1], origin[2]);
       whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
       // fill the image with foreground voxels:
       unsigned char inval = 1;
       unsigned char outval = 0;
       vtkIdType count = whiteImage->GetNumberOfPoints();
       for (vtkIdType i = 0; i < count; ++i)
         {
         whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
         }

       vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();


       // sweep polygonal data (this is the important thing with contours!)
       vtkSmartPointer<vtkLinearExtrusionFilter> extruder = vtkSmartPointer<vtkLinearExtrusionFilter>::New();



       extruder->SetInputData(inData);
       extruder->SetScaleFactor(1.);
       extruder->SetExtrusionTypeToNormalExtrusion();



       double norm[3];
       norm[0] = Vector[0]*spacing[0];
       norm[1] = Vector[1]*spacing[1];
       norm[2] = Vector[2]*spacing[2];
       vtkMath::MultiplyScalar(norm, 0.5);
       extruder->SetVector(norm[0], norm[1], norm[2]);
       extruder->Update();

       append->AddInputData(extruder->GetOutput());

       {
           // sweep polygonal data (this is the important thing with contours!)
           vtkSmartPointer<vtkLinearExtrusionFilter> extruder = vtkSmartPointer<vtkLinearExtrusionFilter>::New();

           extruder->SetInputData(inData);
           extruder->SetScaleFactor(1.);
           extruder->SetExtrusionTypeToNormalExtrusion();
           vtkMath::MultiplyScalar(norm, -1);
           extruder->SetVector(norm[0], norm[1], norm[2]);
           extruder->Update();
           append->AddInputData(extruder->GetOutput());
       }

       // polygonal data --> image stencil:
       vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
       pol2stenc->SetTolerance(0); // important if extruder->SetVector(0, 0, 1) !!!
       pol2stenc->SetInputConnection(append->GetOutputPort());
       pol2stenc->SetOutputOrigin(origin[0], origin[1], origin[2]);
       pol2stenc->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
       pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
       pol2stenc->Update();

       // cut the corresponding white image and set the background:
       vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
       imgstenc->SetInputData(whiteImage);
       imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
       imgstenc->SetOutput(outData);
       imgstenc->ReverseStencilOff();
       imgstenc->SetBackgroundValue(outval);
       imgstenc->Update();

        return 1;
}

 void ContourToImageFilter::PrintSelf(ostream& os, vtkIndent indent){
     os << indent << "ContourToImageFilter"<<endl;
     vtkImageAlgorithm::PrintSelf(os, indent);
 }
