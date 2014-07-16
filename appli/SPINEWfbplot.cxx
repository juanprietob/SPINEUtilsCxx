/*
  Author: Juan Carlos Prieto, SPINE execution of Weighted functional box plots
  Date: 07/10/14
*/

#include <stdlib.h>
#include "wfbplot.h"
#include "gaussweight.h"

#include "vtkPolyData.h"
#include "vtkPolyDataCollection.h"
#include "vtkSmartPointer.h"
#include "spinecontourswriter.h"
#include "vtkPoints.h"
#include "vtkLine.h"
#include "vtkCellArray.h"

using namespace std;

int main(int argv, char **argc)
{

    matrix* data;
    int pDim = 0;
    int nData = 0;
    string outfilename = "";

    if(argc[1]){
        for(unsigned i = 0; i < argv; i++){
            string inputstring = argc[i];

            if(inputstring.compare("--row")==0){
                i++;
                inputstring = argc[i];
                pDim = 3*atoi(inputstring.c_str());
            }
            if(inputstring.compare("--col")==0){
                i++;
                inputstring = argc[i];
                nData = atoi(inputstring.c_str());
            }
            if(inputstring.compare("--data")==0){
                data = new matrix(nData, pDim);
                for(unsigned row = 0; row < nData; row++){
                    for(unsigned col = 0; col < pDim; col++){
                        i++;
                        inputstring = argc[i];
                        double coord = atof(inputstring.c_str());
                        data->setElement(row, col, coord);
                    }
                }
            }
            if(inputstring.compare("--outfn")==0){
                i++;
                outfilename = argc[i];
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
                for(unsigned i = 0; i < nData; i++){
                    for(unsigned j = 0; j < pDim; j++){
                        cin >> inputstring;
                        double coord = atof(inputstring.c_str());
                        data->setElement(i, j, coord);
                    }
                }
            }
            if(inputstring.compare("--outfn")==0){
                cin >> outfilename;
            }
        }
    }


    wfbplot wfbTest(*data);
    //wfbTest.print();

    double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
    wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);

    vector< double* > bplotdata;
    bplotdata.push_back(med);
    bplotdata.push_back(inf);
    bplotdata.push_back(sup);
    bplotdata.push_back(minBd);
    bplotdata.push_back(maxBd);
    bplotdata.push_back(depth);


    vtkSmartPointer<vtkPolyDataCollection> boxplotcollection = vtkSmartPointer<vtkPolyDataCollection>::New();
    for(unsigned i=0; i < bplotdata.size(); i++){

        vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();

        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> cellarray = vtkSmartPointer<vtkCellArray>::New();


        for(int j = 1; j < pDim/3; j++){
            if(j == 1){
                points->InsertNextPoint(bplotdata[i][0], bplotdata[i][1], bplotdata[i][2]);
            }

            points->InsertNextPoint(bplotdata[i][j*3], bplotdata[i][j*3 + 1], bplotdata[i][j*3 + 2]);

            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, j - 1);
            line->GetPointIds()->SetId(1, j);

            cellarray->InsertNextCell(line);

            if(j == pDim - 1){
                vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
                line->GetPointIds()->SetId(0, j);
                line->GetPointIds()->SetId(1, 0);

                cellarray->InsertNextCell(line);
            }

        }
        poly->SetPoints(points);
        poly->SetLines(cellarray);

        boxplotcollection->AddItem(poly);
    }

    vtkSmartPointer<SPINEContoursWriter> writer = vtkSmartPointer<SPINEContoursWriter>::New();
    writer->SetInputData(boxplotcollection);
    if(outfilename.compare("")!=0){
        writer->SetFileName(outfilename.c_str());
    }
    writer->Write();

    /*if(outfilename.compare("") != 0){
        wfbTest.printVector(pDim, med);
        wfbTest.printVector(pDim, inf);
        wfbTest.printVector(pDim, sup);
        wfbTest.printVector(pDim, minBd);
        wfbTest.printVector(pDim, maxBd);
        wfbTest.printVector(nData, depth);
    }*/





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



