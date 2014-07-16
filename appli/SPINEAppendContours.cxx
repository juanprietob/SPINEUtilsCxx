/*
  Author: Juan Carlos Prieto, SPINE execution of Weighted functional box plots
  Date: 07/10/14
*/

#include <stdlib.h>

#include "vtkPolyData.h"
#include "vtkPolyDataCollection.h"
#include "vtkSmartPointer.h"
#include "spinecontourswriter.h"
#include "SPINEContoursReader.h"
#include "vtkPoints.h"
#include "vtkLine.h"
#include "vtkCellArray.h"

using namespace std;

int main(int argv, char **argc)
{


    vector< string > contoursFileNames;
    vector< int > contoursIds;
    string outfilename = "";

    for(int i = 1; i < argv; i++){
        string input = argc[i];
        if(input.compare("-f")==0){
            i++;
            contoursFileNames.push_back(string(argc[i]));
        }else if(input.compare("-id")==0){
            i++;
            contoursIds.push_back(atoi(argc[i]));
        }else if(input.compare("--outfn")==0){
            i++;
            outfilename = string(argc[i]);
        }

    }




    vtkSmartPointer<vtkPolyDataCollection> outputdatacollection = vtkSmartPointer<vtkPolyDataCollection>::New();

    for(unsigned i = 0; i < contoursFileNames.size(); i++){
        string filename = contoursFileNames[i];
        vtkSmartPointer<SPINEContoursReader> reader = vtkSmartPointer<SPINEContoursReader>::New();
        reader->SetFileName(filename.c_str());
        reader->Update();

        vtkPolyDataCollection* currentcollection = reader->GetOutput();

        if(currentcollection->GetNumberOfItems() > contoursIds[i]){
            cout<<"ContourId= "<<contoursIds[i]<<", not in contours= "<<filename;
            return EXIT_FAILURE;
        }

        vtkSmartPointer<vtkPolyData> currentcontour = (vtkPolyData*)currentcollection->GetItemAsObject(contoursIds[i]);
        outputdatacollection->AddItem(currentcontour);

    }

    vtkSmartPointer<SPINEContoursWriter> writer = vtkSmartPointer<SPINEContoursWriter>::New();
    writer->SetInputData(outputdatacollection);
    if(outfilename.compare("")!=0){
        writer->SetFileName(outfilename.c_str());
    }
    writer->Write();


    return EXIT_SUCCESS;
}



