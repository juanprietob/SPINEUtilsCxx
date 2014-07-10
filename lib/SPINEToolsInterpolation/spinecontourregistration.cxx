#include "spinecontourregistration.h"

#include "vtkIterativeClosestPointTransform.h"
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <math.h>
#include <vtkMath.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(SPINEContourRegistration);
using namespace std;

SPINEContourRegistration::SPINEContourRegistration()
{
    SimilarityTransform = false;
}

SPINEContourRegistration::~SPINEContourRegistration(){

}

void SPINEContourRegistration::PrintSelf(ostream& os, vtkIndent indent){

}

int SPINEContourRegistration::RequestData(vtkInformation *vtkNotUsed(request),
                                          vtkInformationVector **inputVector,
                                          vtkInformationVector *outputVector){

    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and output
    vtkSmartPointer<vtkPolyData> source = vtkPolyData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPolyData *target = this->InputTarget;

    if(SimilarityTransform){
        vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
        icp->SetSource(source);
        icp->SetTarget(target);
        icp->GetLandmarkTransform()->SetModeToSimilarity();
        icp->SetMaximumNumberOfIterations(10);
        icp->StartByMatchingCentroidsOn();
        icp->Modified();
        icp->Update();

        // Get the resulting transformation matrix (this matrix takes the source points to the target points)
        //vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
        //std::cout << "The resulting matrix is: " << *m << std::endl;

        vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        icpTransformFilter->SetInputData(source);
        icpTransformFilter->SetTransform(icp);
        icpTransformFilter->Update();

        source = icpTransformFilter->GetOutput();
    }


    int angle = 0;
    double minangle = 0;
    double minDistance = DBL_MAX;
    double stepi = 1;
    int dir = -1;
    int mindir = -1;

    while(dir <= 1){

        while(angle < 360){
            double distance = 0;
            int start = 0;
            int deltai = 1;
            if(dir == -1){
                start = source->GetNumberOfPoints()-1;
                deltai = -1;
            }else{
                start = 0;
                deltai = 1;
            }
            for(int i = start, ii = 0; ii < source->GetNumberOfPoints() ; i+=deltai, ii++){
                int sourceindex = round(angle/360.0*source->GetNumberOfPoints() + i);
                if(sourceindex > source->GetNumberOfPoints()-1){
                    sourceindex -= source->GetNumberOfPoints();
                }
                int targetindex = round(ii/((double)source->GetNumberOfPoints()-1.0) * (target->GetNumberOfPoints()-1));

                double ps[3], pt[3];
                source->GetPoint(sourceindex, ps);
                target->GetPoint(targetindex, pt);

                distance += sqrt(vtkMath::Distance2BetweenPoints(pt, ps));
            }

            if(distance < minDistance){
                minangle = angle;
                minDistance = distance;
                mindir = dir;
            }
            angle++;
        }
        dir+=2;
    }

    vtkSmartPointer<vtkPoints> outputpoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> outputcellarray = vtkSmartPointer<vtkCellArray>::New();

    int start = 0;
    int deltai = 1;
    if(mindir == -1){
        start = source->GetNumberOfPoints()-1;
        deltai = -stepi;
    }else{
        start = 0;
        deltai = stepi;
    }

    for(int i = start, ii = 0; ii < source->GetNumberOfPoints() ; i+=deltai, ii+=stepi){
        int sourceindex = round(minangle/360.0*source->GetNumberOfPoints() + i);
        if(sourceindex > source->GetNumberOfPoints() - 1){
            sourceindex -= source->GetNumberOfPoints();
        }

        int targetindex = round(ii/((double)source->GetNumberOfPoints()-1) * (target->GetNumberOfPoints()-1));
        if(targetindex >= target->GetNumberOfPoints()){
            targetindex = 0;
        }
        double ps[3], pt[3];
        source->GetPoint(sourceindex, ps);
        outputpoints->InsertNextPoint(ps[0], ps[1], ps[2]);
    }

    for(int i = 0; i < source->GetNumberOfPoints(); i++){
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);
        if(i == source->GetNumberOfPoints() - 1){
            line->GetPointIds()->SetId(1, 0);
        }else{
            line->GetPointIds()->SetId(1, i+1);
        }

        outputcellarray->InsertNextCell(line);
    }

    output->SetPoints(outputpoints);
    output->SetLines(outputcellarray);

}
