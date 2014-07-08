



#include <iostream>


#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCollectionIterator.h>

#include <SPINEContoursReader.h>
#include <spinecontoursinterpolation.h>
#include <vtkProperty.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>

using namespace std;

int main(int argv, char** argc){

    cout<<argc[1]<<endl;

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->SetBackground(.3, .6, .3); // Background color green


    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();
    sourcereader->SetFileName(argc[1]);
    sourcereader->DebugOn();

    sourcereader->Update();

    vtkPolyDataCollection* contours = sourcereader->GetOutput();
    vtkCollectionSimpleIterator it;
    contours->InitTraversal(it);

    vtkSmartPointer<vtkPolyData> target = 0;

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        contourinterpolation->SetInputData(nextpoly);
        contourinterpolation->Update();


        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(contourinterpolation->GetOutput());

        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        renderer->AddActor(actor);

        if(i == 0){
            target = contourinterpolation->GetOutput();
        }

        if(i > 0){
            vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
            icp->SetSource(contourinterpolation->GetOutput());
            icp->SetTarget(target);
            icp->GetLandmarkTransform()->SetModeToRigidBody();
            icp->SetMaximumNumberOfIterations(10);
            icp->StartByMatchingCentroidsOn();
            icp->Modified();
            icp->Update();

            // Get the resulting transformation matrix (this matrix takes the source points to the target points)
            vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
            std::cout << "The resulting matrix is: " << *m << std::endl;

            vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            icpTransformFilter->SetInputData(contourinterpolation->GetOutput());
            icpTransformFilter->SetTransform(icp);
            icpTransformFilter->Update();

            vtkSmartPointer<vtkPolyDataMapper> mapperreg = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperreg->SetInputData(icpTransformFilter->GetOutput());

            vtkSmartPointer<vtkActor> actorreg = vtkSmartPointer<vtkActor>::New();
            actorreg->SetMapper(mapperreg);
            actorreg->GetProperty()->SetColor(255,0,255);

            renderer->AddActor(actorreg);





        }

    }

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}

