/*
  Author: Juan Carlos Prieto, SPINE execution of Weighted functional box plots
  Date: 07/10/14
*/

#include <stdlib.h>
#include "wfbplot.h"
#include "gaussweight.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCollectionIterator.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>

using namespace std;

int main(int argv, char **argc)
{

    matrix* data;
    int pDim = 0;
    int nData = 0;

    if(argc[1]){
        for(int i = 0; i < argv; i++){
            string inputstring = argc[i];

            if(inputstring.compare("--row")==0){
                inputstring = argc[i+1];
                pDim = atoi(inputstring.c_str());
            }
            if(inputstring.compare("--col")==0){
                inputstring = argc[i+1];
                nData = atoi(inputstring.c_str());
            }
            if(inputstring.compare("--data")==0){
                data = new matrix(nData, pDim);
                for(int row = 0; row < nData; row++){
                    for(int col = 0; col < pDim; col++){
                        i++;
                        if(i < argv){
                            double coord = atof(argc[i]);
                            data->setElement(row, col, coord);
                            cout<<row<<", "<<col<<"= "<<coord<<endl;
                        }else{
                            cout<<row<<", "<<col<<"= "<<endl;
                        }
                    }
                }
            }
        }

    }else{

        while(!cin.eof()){
            string inputstring;
            cin >> inputstring;

            if(inputstring.compare("--row")==0){
                cin >> inputstring;
                pDim = 3*atoi(inputstring.c_str());
            }
            if(inputstring.compare("--col")==0){
                cin >> inputstring;
                nData = atoi(inputstring.c_str());
            }
            if(inputstring.compare("--data")==0){
                data = new matrix(nData, pDim);
                for(int i = 0; i < nData; i++){
                    for(int j = 0; j < pDim; j++){
                        if(!cin.eof()){
                            cin >> inputstring;
                            double coord = atof(inputstring.c_str());
                            data->setElement(i, j, coord);
                        }else{
                            cout<<i<<" "<<j<<endl;
                        }
                    }
                }
            }
        }
    }


    wfbplot wfbTest(*data);
    wfbTest.print();

    double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
    wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);
    wfbTest.printVector(pDim, med);
    wfbTest.printVector(pDim, inf);
    wfbTest.printVector(pDim, sup);
    wfbTest.printVector(pDim, minBd);
    wfbTest.printVector(pDim, maxBd);
    wfbTest.printVector(nData, depth);


    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
    renderWindowInteractor->SetInteractorStyle(style);
    renderer->SetBackground(.3, .6, .3); // Background color green


    for(unsigned j = 0; j < nData; j++){
        vtkSmartPointer<vtkPolyData> contourpoly = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPoints> contourpoints = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> contourcells = vtkSmartPointer<vtkCellArray>::New();
        for(unsigned i = 0; i < pDim; i+=3){
            contourpoints->InsertNextPoint(data->getElement(j, i), data->getElement(j, i+1), data->getElement(j, i+2));
        }

        for(unsigned i = 0; i < contourpoints->GetNumberOfPoints(); i++){
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, i);
            if(i == contourpoints->GetNumberOfPoints() - 1){
              line->GetPointIds()->SetId(1, 0);
            }else{
                line->GetPointIds()->SetId(1, i+1);
            }
            contourcells->InsertNextCell(line);
        }
        contourpoly->SetPoints(contourpoints);
        contourpoly->SetLines(contourcells);

        vtkSmartPointer<vtkPolyDataMapper> contourmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        contourmapper->SetInputData(contourpoly);

        vtkSmartPointer<vtkActor> contouractor = vtkSmartPointer<vtkActor>::New();
        contouractor->SetMapper(contourmapper);
        double col = ((double)j+1)/((double)nData);
        contouractor->GetProperty()->SetColor(col, 0.2, 0.5);

        renderer->AddActor(contouractor);
    }


    vtkSmartPointer<vtkPolyData> medianpoly = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> medianpoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> mediancells = vtkSmartPointer<vtkCellArray>::New();

    vtkSmartPointer<vtkPolyData> infpoly = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> infpoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> infcells = vtkSmartPointer<vtkCellArray>::New();

    vtkSmartPointer<vtkPolyData> suppoly = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> suppoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> supcells = vtkSmartPointer<vtkCellArray>::New();


    for(unsigned i = 0; i < pDim; i+=3){
        medianpoints->InsertNextPoint(med[i], med[i+1], med[i+2]);
        infpoints->InsertNextPoint(minBd[i], minBd[i+1], minBd[i+2]);
        suppoints->InsertNextPoint(maxBd[i], maxBd[i+1], maxBd[i+2]);
    }

    for(unsigned i = 0; i < medianpoints->GetNumberOfPoints(); i++){
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);
        if(i == medianpoints->GetNumberOfPoints() - 1){
          line->GetPointIds()->SetId(1, 0);
        }else{
            line->GetPointIds()->SetId(1, i+1);
        }
        mediancells->InsertNextCell(line);
        infcells->InsertNextCell(line);
        supcells->InsertNextCell(line);
    }
    medianpoly->SetPoints(medianpoints);
    medianpoly->SetLines(mediancells);

    infpoly->SetPoints(infpoints);
    infpoly->SetLines(infcells);
    suppoly->SetPoints(suppoints);
    suppoly->SetLines(supcells);

    vtkSmartPointer<vtkPolyDataMapper> medianmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    medianmapper->SetInputData(medianpoly);

    vtkSmartPointer<vtkActor> medianactor = vtkSmartPointer<vtkActor>::New();
    medianactor->SetMapper(medianmapper);
    medianactor->GetProperty()->SetColor(0, 1, 1);

    vtkSmartPointer<vtkPolyDataMapper> infmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    infmapper->SetInputData(infpoly);

    vtkSmartPointer<vtkActor> infactor = vtkSmartPointer<vtkActor>::New();
    infactor->SetMapper(infmapper);
    infactor->GetProperty()->SetColor(1, 0, 1);

    vtkSmartPointer<vtkPolyDataMapper> supmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    supmapper->SetInputData(suppoly);

    vtkSmartPointer<vtkActor> supactor = vtkSmartPointer<vtkActor>::New();
    supactor->SetMapper(supmapper);
    supactor->GetProperty()->SetColor(0, 1, 1);

    //renderer->AddActor(medianactor);
    renderer->AddActor(infactor);
    renderer->AddActor(supactor);

    renderWindow->Render();
    renderWindowInteractor->Start();


    delete data;

    // built atlas using weighted functional boxplot
    //cout<<" --double sigma = 30;
    /*for(int id=0; id < nData; id++)
    {
        //double weight[nData];
        // compute weight
        //gaussweight::computeModifiedGaussian( ages, nData, ages[id], sigma, ageRange[0], ageRange[1], weight);
        //wfbTest.setWeight(weight);
        //wfbTest.printVector(nData, weight);
        double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
        // call weight functional boxplot function
        wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);
        wfbTest.printVector(nData, depth);
        wfbTest.printVector(pDim, med);
        wfbTest.printVector(pDim, inf);
        wfbTest.printVector(pDim, sup);
        wfbTest.printVector(pDim, minBd);
        wfbTest.printVector(pDim, maxBd);
    }*/

    return EXIT_SUCCESS;
}




