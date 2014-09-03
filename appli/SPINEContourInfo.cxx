

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

#include "itkMesh.h"
#include "itkVTKPolyDataMeshIO.h"

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

    vector< vtkSmartPointer< SPINEContoursInterpolation > > vectorinterpolation;

    double contourBB[6] = {VTK_DOUBLE_MAX, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, VTK_DOUBLE_MIN};

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        if(nextpoly->GetPointData()->GetAbstractArray("boxplotsarea") && nextpoly->GetPointData()->GetAbstractArray("boxplotsname")){
            vtkStringArray* bplotsname = dynamic_cast<vtkStringArray*>(nextpoly->GetPointData()->GetAbstractArray("boxplotsname"));
            vtkDoubleArray* bplotsarea = dynamic_cast<vtkDoubleArray*>(nextpoly->GetPointData()->GetAbstractArray("boxplotsarea"));

            cout<<"{"<<endl;
            for(unsigned i = 0; i < bplotsname->GetNumberOfValues(); i++){

                cout<<"\""<<bplotsname->GetValue(i)<<"\":"<<"\""<<bplotsarea->GetValue(i)<<"\"";
                if(i < bplotsname->GetNumberOfValues() - 1){
                    cout<<",";
                }
                cout<<endl;

            }
            cout<<"}"<<endl;

        }else{
            vtkSmartPointer<SPINEContoursInterpolation> interpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
            interpolation->SetInputData(nextpoly);

            interpolation->Update();

            vectorinterpolation.push_back(interpolation);

            double bounds[6];

            nextpoly->GetBounds(bounds);

            for(int j = 0; j < 6; j+=2){
                if(bounds[j] < contourBB[j]){
                    contourBB[j] = bounds[j];
                }
            }

            for(int j = 1; j < 6; j+=2){
                if(bounds[j] > contourBB[j]){
                    contourBB[j] = bounds[j];
                }
            }
        }
    }

    /*cout<<"{";
    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

        vtkSmartPointer<SPINEContoursInterpolation> contourinterpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        contourinterpolation->SetInputData(nextpoly);
        contourinterpolation->Update();

        cout<<"{\"id\": \""<<i<<"\", ";
        cout<<"\"area\": \""<<contourinterpolation->GetArea()<<"\", ";
        cout<<"\"perimeter\": \""<<contourinterpolation->GetContourLength()<<"\"";
        cout<<"}";

    }
    cout<<"}";
    cout<<endl;*/

    if(vectorinterpolation.size() > 2){
        vector< vtkSmartPointer<vtkImageData> > imagevector;

        double *bounds = contourBB;
        double spacing[3];
        spacing[0] = 0.25;
        spacing[1] = 0.25;
        spacing[2] = 0.25;
        // compute dimensions
        int dim[3];
        for (int i = 0; i < 3; i++)
        {
          dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) /
              spacing[i]));
            if (dim[i] < 1){
                dim[i] = 1;
            }

        }

        double origin[3];
        // NOTE: I am not sure whether or not we had to add some offset!
        origin[0] = bounds[0];// + spacing[0] / 2;
        origin[1] = bounds[2];// + spacing[1] / 2;
        origin[2] = bounds[4];// + spacing[2] / 2;

        for(int i = 0; i < vectorinterpolation.size(); i++){

            double *avgnorm = vectorinterpolation[i]->GetAvgNormal();

            vtkSmartPointer<ContourToImageFilter> contourtoimage = vtkSmartPointer<ContourToImageFilter>::New();
            contourtoimage->SetInputConnection(vectorinterpolation[i]->GetOutputPort());
            contourtoimage->SetVector(avgnorm);
            contourtoimage->SetDimensions(dim);
            contourtoimage->SetSpacing(spacing);
            contourtoimage->SetOrigin(origin);

            contourtoimage->Update();

            imagevector.push_back(contourtoimage->GetOutput());


        }

        int pDim = dim[0]*dim[1]*dim[2];

        int nData = imagevector.size();

        matrix* data = new matrix(nData, pDim);

        for(int i = 0; i < nData; i++){
            unsigned char*ptr = (unsigned char*)imagevector[i]->GetScalarPointer();
            for(int j = 0; j < pDim; j++){

                double temp = (double)*ptr;
                data->setElement(i, j, temp);
                ++ptr;
            }
        }

        wfbplot wfbTest(*data);
        //wfbTest.print();

        double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
        wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);

        vector< double* > bplotdata;

        bplotdata.push_back(maxBd);
        bplotdata.push_back(sup);
        bplotdata.push_back(med);
        bplotdata.push_back(inf);
        bplotdata.push_back(minBd);
        //bplotdata.push_back(depth);

        vtkSmartPointer<vtkStringArray> bplotdatanames = vtkSmartPointer<vtkStringArray>::New();
        bplotdatanames->SetName("boxplotsname");
        bplotdatanames->InsertValue(0, "maxBd");
        bplotdatanames->InsertValue(1, "sup");
        bplotdatanames->InsertValue(2, "med");
        bplotdatanames->InsertValue(3, "inf");
        bplotdatanames->InsertValue(4, "minBd");

        typedef itk::Image< unsigned short, 3> ImageType;
        typedef itk::ImageRegionIterator< ImageType > ImageIteratorType;

        typedef itk::Image< unsigned short, 3> OutputImageType;

        typedef EdgeContourFilter< ImageType, OutputImageType > ContourDetectionFilter;


        ImageType::RegionType region;
        itk::Size<3> size;
        size.SetElement(0, dim[0]);
        size.SetElement(1, dim[1]);
        size.SetElement(2, dim[2]);
        region.SetSize(size);

        vtkSmartPointer< vtkDoubleArray > bplotarea = vtkSmartPointer< vtkDoubleArray >::New();
        bplotarea->SetName("boxplotsarea");

        cout<<"{"<<endl;
        for(unsigned i = 0; i < bplotdata.size(); i++){

            ImageType::Pointer img = ImageType::New();

            img->SetRegions(region);
            img->SetSpacing(spacing);
            img->SetOrigin(origin);
            img->Allocate();
            img->FillBuffer(0);

            ImageIteratorType it(img, img->GetLargestPossibleRegion());
            it.GoToBegin();

            for(int j = 0; j < pDim && !it.IsAtEnd(); j++){
                it.Set(bplotdata[i][j]);
                ++it;
            }

            ContourDetectionFilter::Pointer contourFilter = ContourDetectionFilter::New();
            contourFilter->SetInput(img);
            contourFilter->Update();
            img = contourFilter->GetOutput();

            /*char outfilename[50];
            sprintf(outfilename, "contour%d.nii.gz", i);

            typedef itk::ImageFileWriter< OutputImageType > ImageFileWriterType;
            ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
            writer->SetInput(img);
            writer->SetFileName(outfilename);
            writer->Update();*/

            int j = 0;
            double numpix = 0;
            while(j < pDim){
                if(bplotdata[i][j] != 0){
                    numpix++;
                }
                j++;
            }

            double area = numpix*spacing[0]*spacing[1];
            //cout<<numpix<<","<<spacing[0]*spacing[1];
            double perimeter = 0;

            {
                ImageIteratorType it(img, img->GetLargestPossibleRegion());
                it.GoToBegin();
                numpix = 0;
                while(!it.IsAtEnd()){
                    if(it.Get() != 0){
                        numpix++;
                    }
                    ++it;
                }
                perimeter = numpix*0.25;
                //cout<<", "<<numpix<<", "<<sqrt(pow(spacing[0], 2) + pow(spacing[1], 2));

            }

            cout<<"\""<<bplotdatanames->GetValue(i)<<"\":"<<"{\"area\" : \""<<area<<"\", \"perimeter\" : \""<<perimeter<<"\"}";
            if(i < bplotdata.size() - 1){
                cout<<",";
            }
            cout<<endl;



        }
        cout<<"}"<<endl;

        delete data;
    }



    return EXIT_SUCCESS;

}






