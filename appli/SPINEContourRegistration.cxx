



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

using namespace std;

int main(int argv, char** argc){

    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();
    //cout<<argc[0]<<endl;

    string filename = "";

    if(argc[1]){
        cout<<argc[1]<<endl;
        filename = argc[1];
        sourcereader->SetFileName(filename.c_str());
    }else{
        string contourXML;
        while(!cin.eof()){
            string inputstring;
            cin >> inputstring;
            contourXML += " " + inputstring + " ";
        }

        sourcereader->SetFileContent(contourXML.c_str());

    }



    //sourcereader->DebugOn();


    sourcereader->Update();

    vtkPolyDataCollection* contours = sourcereader->GetOutput();
    vtkCollectionSimpleIterator it;
    contours->InitTraversal(it);

    vtkSmartPointer<vtkPolyData> target = 0;
    double contourlength = 0;
    int numsamples = 200;


    vector< vtkSmartPointer< vtkPolyData> > vectorsource;

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        contourinterpolation->SetInputData(nextpoly);
        contourinterpolation->Update();

        //cout<<"Contour length = "<<contourinterpolation->GetContourLength()<<endl;
        //cout<<"Num points = "<<contourinterpolation->GetOutput()->GetNumberOfPoints()<<endl;

        vectorsource.push_back(contourinterpolation->GetOutput());

        if(contourlength < contourinterpolation->GetContourLength()){
            target = contourinterpolation->GetOutput();
            contourlength = contourinterpolation->GetContourLength();
        }
    }

    //cout<<endl;
    for(unsigned i = 0; i < vectorsource.size(); i++){

        vtkSmartPointer<SPINEContourRegistration> regcontours = vtkSmartPointer<SPINEContourRegistration>::New();
        regcontours->SetInputData(vectorsource[i]);
        regcontours->SetInputTarget(target);
        regcontours->SetSimilarityTransform(true);
        regcontours->Update();
        vtkPolyData* source = regcontours->GetOutput();

        int stepj = source->GetNumberOfPoints()/numsamples;

        for(int j = 0; j < source->GetNumberOfPoints() ; j+=stepj){

            double ps[3];
            source->GetPoint(j, ps);

            cout<<ps[0]<<" "<<ps[1]<<" "<<ps[2]<<" ";

        }
        cout<<endl;

    }
    cout<<endl;

    return EXIT_SUCCESS;

}


