



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
#include "contourtoimagefilter.h"

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkAddImageFilter.h"

using namespace std;


int main(int argv, char** argc){

    vtkSmartPointer<SPINEContoursReader> sourcereader = vtkSmartPointer<SPINEContoursReader>::New();
    //cout<<argc[0]<<endl;

    string filename = "";

    if(argv > 1){
        //cout<<argc[1]<<endl;
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

    vtkSmartPointer<vtkPolyData> target = 0;
    double contourlength = 0;


    vector< vtkSmartPointer< SPINEContoursInterpolation > > vectorinterpolation;

    double contourBB[6] = {VTK_DOUBLE_MAX, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, VTK_DOUBLE_MIN};

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);

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
        if (dim[i] == 1){
            spacing[i] = 1;
        }
    }

    double origin[3];
    // NOTE: I am not sure whether or not we had to add some offset!
    origin[0] = bounds[0];// + spacing[0] / 2;
    origin[1] = bounds[2];// + spacing[1] / 2;
    origin[2] = bounds[4];// + spacing[2] / 2;

    vector< vtkSmartPointer<vtkImageData> > imagevector;

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

    /*for(int i = 0; i < imagevector.size(); i++){
        vtkSmartPointer<vtkMetaImageWriter> imageWriter = vtkSmartPointer<vtkMetaImageWriter>::New();

        char buf[50];
        sprintf(buf, "testImgS%d.mhd", i);

        imageWriter->SetFileName(buf);
        imageWriter->SetInputData(imagevector[i]);
        imageWriter->Write();
    }*/

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

    unsigned short minVal = 0, maxVal = 5;//Background value and minBd value
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
    /*bplotdatanames.push_back("maxBd");
    bplotdatanames.push_back("sup");
    bplotdatanames.push_back("med");
    bplotdatanames.push_back("inf");
    bplotdatanames.push_back("minBd");*/



    //bplotdatanames.push_back("depth");

    typedef itk::Image< unsigned short, 3> ImageType;
    typedef itk::ImageRegionIterator< ImageType > ImageIteratorType;

    ImageType::Pointer resimg = ImageType::New();

    ImageType::RegionType region;
    itk::Size<3> size;
    size.SetElement(0, dim[0]);
    size.SetElement(1, dim[1]);
    size.SetElement(2, dim[2]);
    region.SetSize(size);

    resimg->SetRegions(region);
    resimg->SetSpacing(spacing);
    resimg->SetOrigin(origin);
    resimg->Allocate();
    resimg->FillBuffer(0);


    vtkSmartPointer< vtkDoubleArray > bplotarea = vtkSmartPointer< vtkDoubleArray >::New();
    bplotarea->SetName("boxplotsarea");
    for(unsigned i = 0; i < bplotdata.size(); i++){


        ImageType::Pointer img = ImageType::New();

        ImageType::RegionType region;
        itk::Size<3> size;
        size.SetElement(0, dim[0]);
        size.SetElement(1, dim[1]);
        size.SetElement(2, dim[2]);
        region.SetSize(size);

        img->SetRegions(region);
        img->SetSpacing(spacing);
        img->SetOrigin(origin);
        img->Allocate();



        ImageIteratorType it(img, img->GetLargestPossibleRegion());
        it.GoToBegin();
        int j = 0;
        double numpix = 0;
        while(!it.IsAtEnd() && j < pDim){
            it.Set(bplotdata[i][j]);
            if(bplotdata[i][j] != 0){
                numpix++;
            }
            j++;
            ++it;
        }

        typedef itk::AddImageFilter< ImageType > AddImageFilterType;
        AddImageFilterType::Pointer addimage = AddImageFilterType::New();
        addimage->SetInput1(resimg);
        addimage->SetInput2(img);
        addimage->Update();
        resimg = addimage->GetOutput();
        numpix = numpix*spacing[0]*spacing[1]*spacing[2];

        bplotarea->InsertNextValue(numpix);

        /*typedef itk::BinaryContourImageFilter< ImageType, ImageType > BinaryContourFilterType;
        BinaryContourFilterType::Pointer binarycontour = BinaryContourFilterType::New();
        binarycontour->SetInput(img);
        binarycontour->Update();*/

        /*typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
        ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
        writer->SetInput(binarycontour->GetOutput());
        string outfile = bplotdatanames[i] + ".mhd";
        writer->SetFileName(outfile.c_str());
        writer->Update();*/



        /*vtkSmartPointer< vtkImageData > img = vtkSmartPointer< vtkImageData >::New();

        img->SetSpacing(spacing);
        img->SetDimensions(dim);
        img->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);
        img->SetOrigin(origin);
        img->AllocateScalars(VTK_DOUBLE,1);

        double* ptr = (double*)img->GetScalarPointer();
        for(int j = 0; j < pDim; j++){
            *ptr = bplotdata[i][j];
            ++ptr;
        }


        vtkSmartPointer< vtkMetaImageWriter > writer = vtkSmartPointer< vtkMetaImageWriter >::New();
        string outfile = bplotdatanames[i] + ".mhd";
        writer->SetFileName(outfile.c_str());
        writer->SetInputData(img);
        writer->Write();*/
    }

    /*typedef itk::LabelContourImageFilter< ImageType, ImageType > LabelContourFilterType;
    LabelContourFilterType::Pointer labelcontour = LabelContourFilterType::New();
    labelcontour->SetInput(resimg);
    labelcontour->SetFullyConnected(true);
    labelcontour->Update();
    resimg = labelcontour->GetOutput();*/

    ImageIteratorType resit(resimg, resimg->GetLargestPossibleRegion());
    resit.GoToBegin();

    vtkSmartPointer<vtkPolyData> boxplot = vtkSmartPointer<vtkPolyData>::New();

    boxplot->GetPointData()->AddArray(bplotdatanames);
    boxplot->GetPointData()->AddArray(bplotarea);

    vtkSmartPointer<vtkPoints> boxplotpoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedShortArray> labelarray = vtkSmartPointer<vtkUnsignedShortArray>::New();
    labelarray->SetName("labels");

    while(!resit.IsAtEnd()){
        if(resit.Get() != minVal && resit.Get() != maxVal){
            ImageType::PointType physpoint;
            resimg->TransformIndexToPhysicalPoint(resit.GetIndex(), physpoint);
            boxplotpoints->InsertNextPoint(physpoint[0], physpoint[1], physpoint[2]);
            labelarray->InsertNextTuple1(resit.Get());
        }
        ++resit;
    }

    boxplot->SetPoints(boxplotpoints);
    boxplot->GetPointData()->SetScalars(labelarray);

    vtkSmartPointer<vtkPolyDataCollection> boxplotcollection = vtkSmartPointer<vtkPolyDataCollection>::New();
    boxplotcollection->AddItem(boxplot);

    vtkSmartPointer<SPINEContoursWriter> contourwriter = vtkSmartPointer<SPINEContoursWriter>::New();
    contourwriter->SetInputData(boxplotcollection);
    contourwriter->SetContoursType("boxplots");
    contourwriter->Write();

    /*
    string outfilename = "outBPlot.nii.gz";
    typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
    ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
    writer->SetInput(resimg);
    writer->SetFileName(outfilename.c_str());
    writer->Update();*/

    /*if(outfilename.compare("") != 0){
        wfbTest.printVector(pDim, med);
        wfbTest.printVector(pDim, inf);
        wfbTest.printVector(pDim, sup);
        wfbTest.printVector(pDim, minBd);
        wfbTest.printVector(pDim, maxBd);
        wfbTest.printVector(nData, depth);
    }*/



    delete data;

    return EXIT_SUCCESS;

}


