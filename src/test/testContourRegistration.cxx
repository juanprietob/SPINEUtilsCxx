



#include <iostream>


#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCollectionIterator.h>

#include <SPINEContoursReader.h>
#include <spinecontoursinterpolation.h>
#include <vtkProperty.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPointLocator.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>

#include <spinecirclecenter.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <minimizecircledistance.h>
#include <spinecontourregistration.h>
#include <vtkMath.h>
#include <vtkPointData.h>

using namespace std;

int main(int argv, char** argc){

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
    renderWindowInteractor->SetInteractorStyle(style);
    renderer->SetBackground(.3, .6, .3); // Background color green

    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();

    if(argv > 1){
        sourcereader->SetFileName(argc[1]);
    }else{
        string contourXML = "";
        while(!cin.eof()){
            string inputstring = "";
            cin >> inputstring;
            contourXML += inputstring + " ";
        }

        //cout<<contourXML<<endl;

        sourcereader->SetFileContent(contourXML.c_str());
    }

    sourcereader->Update();

    vtkPolyDataCollection* contours = sourcereader->GetOutput();
    vtkCollectionSimpleIterator it;
    contours->InitTraversal(it);

    vtkSmartPointer<vtkPolyData> target = 0;
    double contourlength = 0;
    int numsamples = 200;


    vector<vtkPolyData*> vectorsource;

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        contourinterpolation->SetInputData(nextpoly);
        contourinterpolation->Update();

        cout<<"Contour length = "<<contourinterpolation->GetContourLength()<<endl;
        cout<<"Num points = "<<contourinterpolation->GetOutput()->GetNumberOfPoints()<<endl;

        vectorsource.push_back(contourinterpolation->GetOutput());


        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(contourinterpolation->GetOutput());

        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        renderer->AddActor(actor);

        if(contourlength < contourinterpolation->GetContourLength()){
            target = contourinterpolation->GetOutput();
            contourlength = contourinterpolation->GetContourLength();
        }
    }

    for(unsigned i = 0; i < vectorsource.size(); i++){
        if(vectorsource[i] != target){

            vtkSmartPointer<SPINEContourRegistration> regcontours = vtkSmartPointer<SPINEContourRegistration>::New();
            regcontours->SetInputData(vectorsource[i]);
            regcontours->SetInputTarget(target);
            regcontours->SetSimilarityTransform(false);
            regcontours->Update();
            vtkPolyData* source = regcontours->GetOutput();

            vtkSmartPointer<vtkPolyDataMapper> sourcemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            sourcemapper->SetInputData(source);

            vtkSmartPointer<vtkActor> sourceactor = vtkSmartPointer<vtkActor>::New();
            sourceactor->SetMapper(sourcemapper);
            sourceactor->GetProperty()->SetColor(255,0,0);

            renderer->AddActor(sourceactor);


            /*MinimizeCircleDistance mindist;
            mindist.SetSource(source);
            mindist.SetTarget(target);

            vnl_levenberg_marquardt levenberg(mindist);

            vnl_vector<double> anglevnl(1);
            anglevnl.fill(0);

            levenberg.minimize(anglevnl);
            double angle = anglevnl[0];

            while(angle < 0){
                angle += 2*M_PI;
            }

            double delta = angle/(2*M_PI) * source->GetNumberOfPoints();*/

            vtkSmartPointer<vtkPoints> connectingpoints = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkCellArray> connectingcellarray = vtkSmartPointer<vtkCellArray>::New();
            vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
            colors->SetNumberOfComponents(3);
            colors->SetName("Colors");

            int stepi = source->GetNumberOfPoints()/numsamples*8;

            for(unsigned i = 0; i < source->GetNumberOfPoints(); i+=stepi){
            //for(int i = 0; i < 1  ; i+=stepi){

                int targetindex = round(i/((double)source->GetNumberOfPoints()-1) * (target->GetNumberOfPoints()-1));
                if(targetindex >= target->GetNumberOfPoints()){
                    targetindex = 0;
                }
                double ps[3], pt[3];
                source->GetPoint(i, ps);
                target->GetPoint(targetindex, pt);
                vtkIdType id0 = connectingpoints->InsertNextPoint(ps[0], ps[1], ps[2]);
                vtkIdType id1 = connectingpoints->InsertNextPoint(pt[0], pt[1], pt[2]);

                double tempcol = ((double)i)/((double)source->GetNumberOfPoints())*255.0;
                unsigned char co[3] = {0, 0, (unsigned char)tempcol};
                colors->InsertNextTupleValue(co);
                colors->InsertNextTupleValue(co);

                vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
                line->GetPointIds()->SetId(0, id0);
                line->GetPointIds()->SetId(1, id1);

                connectingcellarray->InsertNextCell(line);
            }

            vtkSmartPointer<vtkPolyData> connectingpoly = vtkSmartPointer<vtkPolyData>::New();
            connectingpoly->SetPoints(connectingpoints);
            connectingpoly->SetLines(connectingcellarray);
            connectingpoly->GetPointData()->SetScalars(colors);


            vtkSmartPointer<vtkPolyDataMapper> connectingmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            connectingmapper->SetInputData(connectingpoly);

            vtkSmartPointer<vtkActor> connectingactor = vtkSmartPointer<vtkActor>::New();
            connectingactor->SetMapper(connectingmapper);
            connectingactor->GetProperty()->SetColor(0,255,0);

            renderer->AddActor(connectingactor);


        }
    }

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

}

