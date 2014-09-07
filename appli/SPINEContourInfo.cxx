

#include <iostream>

#include <itkBinaryMask3DMeshSource.h>

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
#include <vtkImageData.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkPointData.h>
#include <vtkAppendPolyData.h>
#include <vtkMetaImageWriter.h>
#include "vtkUnsignedShortArray.h"
#include "vtkStringArray.h"

#include "wfbplot.h"
#include "gaussweight.h"
#include "spinecontourswriter.h"

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "edgecontourfilter.h"
#include "itkImageFileWriter.h"
#include "itkAddImageFilter.h"
#include "contourtoimagefilter.h"

#include "itkShapeLabelMapFilter.h"

using namespace std;


int main(int argv, char** argc){

    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();
    //cout<<argc[0]<<endl;

    string filename = "";

    if(argv > 1){
        filename = argc[1];
        sourcereader->SetFileName(filename.c_str());
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



    //sourcereader->DebugOn();
    sourcereader->Update();

    vtkPolyDataCollection* contours = sourcereader->GetOutput();
    vtkCollectionSimpleIterator it;
    contours->InitTraversal(it);

    cout<<"[";
    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        if(nextpoly->GetPointData()->GetAbstractArray("boxplotsarea") && nextpoly->GetPointData()->GetAbstractArray("boxplotsname") && nextpoly->GetPointData()->GetAbstractArray("boxplotsperimeter")){
            vtkStringArray* bplotsname = dynamic_cast<vtkStringArray*>(nextpoly->GetPointData()->GetAbstractArray("boxplotsname"));
            vtkDoubleArray* bplotsarea = dynamic_cast<vtkDoubleArray*>(nextpoly->GetPointData()->GetAbstractArray("boxplotsarea"));
            vtkDoubleArray* bplotsperimeter = dynamic_cast<vtkDoubleArray*>(nextpoly->GetPointData()->GetAbstractArray("boxplotsperimeter"));


            for(unsigned i = 0; i < bplotsname->GetNumberOfValues(); i++){
                cout<<"{"<<"\"id:\""<<"\""<<bplotsname->GetValue(i)<<"\", ";
                cout<<"{\"area\":"<<"\""<<bplotsarea->GetValue(i)<<"\", ";
                cout<<"\"perimeter\":"<<"\""<<bplotsperimeter->GetValue(i)<<"\"}}";
                if(i < bplotsname->GetNumberOfValues() - 1){
                    cout<<",";
                }
                cout<<endl;

            }
            cout<<"]"<<endl;

        }else if(!(nextpoly->GetPointData()->GetAbstractArray("boxplotsname"))){

            vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
            contourinterpolation->SetInputData(nextpoly);
            contourinterpolation->Update();

            cout<<"{\"id\": \""<<i<<"\", ";
            cout<<"{\"area\": \""<<contourinterpolation->GetArea()<<"\", ";
            cout<<"\"perimeter\": \""<<contourinterpolation->GetContourLength()<<"\"";
            cout<<"}}";
        }

        if(i < contours->GetNumberOfItems() - 1){
            cout<<","<<endl;
        }

    }
    cout<<"]"<<endl;

    return EXIT_SUCCESS;

}






