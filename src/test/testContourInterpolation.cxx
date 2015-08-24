


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
#include <vtkPointData.h>
#include <vtkMath.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkContourFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkLinearExtrusionFilter.h>

using namespace std;

int main(int argv, char** argc){

    cout<<argc[1]<<endl;

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->SetBackground(.3, .6, .3); // Background color green


    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();
    sourcereader->SetFileName(argc[1]);
    sourcereader->DebugOn();

    sourcereader->Update();

    vtkPolyDataCollection* contours = sourcereader->GetOutput();
    vtkCollectionSimpleIterator it;
    contours->InitTraversal(it);

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        vtkSmartPointer<vtkPolyDataMapper> mapper0 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper0->SetInputData(nextpoly);

        vtkSmartPointer<vtkActor> actor0 = vtkSmartPointer<vtkActor>::New();
        actor0->SetMapper(mapper0);
        actor0->GetProperty()->SetColor(255, 0, 255);

        renderer->AddActor(actor0);

        vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        contourinterpolation->SetInputData(nextpoly);
        contourinterpolation->Update();
        vtkPolyData* interpolatedcontour = contourinterpolation->GetOutput();
        cout<<"Contour area= "<<contourinterpolation->GetArea()<<endl;

        {
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(interpolatedcontour);

            vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetRepresentationToWireframe();
            renderer->AddActor(actor);
        }

        // sweep polygonal data (this is the important thing with contours!)
        vtkSmartPointer<vtkLinearExtrusionFilter> extruder = vtkSmartPointer<vtkLinearExtrusionFilter>::New();

        extruder->SetInputData(interpolatedcontour);
        extruder->SetScaleFactor(1.);
        extruder->SetExtrusionTypeToNormalExtrusion();
        extruder->SetVector(-0.5, 0, 0);
        extruder->Update();


        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(extruder->GetOutput());

        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        //renderer->AddActor(actor);



        if(false){//test polynormals
            vtkSmartPointer<vtkPolyData> polynormals = vtkSmartPointer<vtkPolyData>::New();
            vtkSmartPointer<vtkPoints> normalpoints = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkCellArray> cellarray = vtkSmartPointer<vtkCellArray>::New();

            vtkDataArray* normals = contourinterpolation->GetOutput()->GetPointData()->GetNormals();

            for(unsigned j = 0; j < normals->GetNumberOfTuples(); j++){
                double* normal = normals->GetTuple3(i);
                vtkMath::Normalize(normal);
                double p0[3];
                contourinterpolation->GetOutput()->GetPoint(j, p0);
                double p1[3];
                vtkMath::Add(p0, normal, p1);



                vtkSmartPointer<vtkLine> normalline = vtkSmartPointer<vtkLine>::New();
                normalline->GetPointIds()->SetId(0, normalpoints->InsertNextPoint(p0[0], p0[1], p0[2]));
                normalline->GetPointIds()->SetId(1, normalpoints->InsertNextPoint(p1[0], p1[1], p1[2]));

                cellarray->InsertNextCell(normalline);



            }

            polynormals->SetPoints(normalpoints);
            polynormals->SetLines(cellarray);

            {
                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(polynormals);

                vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
                actor->SetMapper(mapper);

                renderer->AddActor(actor);
            }
        }


    }

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}

