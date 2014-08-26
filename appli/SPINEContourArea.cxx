


#include <iostream>


#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkCollectionIterator.h>

#include <SPINEContoursReader.h>
#include <spinecontoursinterpolation.h>

using namespace std;

void help(string exec){
    cerr<<"Usage: "<<exec<<" <contour filename>"<<endl;
    cerr<<"Output: <contour filename>; areaContour1; areaContour2; ... ; areaContourN"<<endl;
}

int main(int argv, char** argc){

    if(argv < 2){
        help(string(argc[0]));

        return 0;
    }
    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();
    sourcereader->SetFileName(argc[1]);
    sourcereader->Update();

    vtkPolyDataCollection* contours = sourcereader->GetOutput();
    vtkCollectionSimpleIterator it;
    contours->InitTraversal(it);

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        cout<<"; ";

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        contourinterpolation->SetInputData(nextpoly);
        contourinterpolation->Update();
        cout<<contourinterpolation->GetArea();

    }
    cout<<endl;


    return EXIT_SUCCESS;

}

