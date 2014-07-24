


#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkPoints.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkCurvatures.h>
#include <vtkPointData.h>

#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>

#include <vtkDoubleArray.h>

#include <vtkImageData.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>

#include <vtkMetaImageWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkQuad.h>

#include <SPINEContoursReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLinearExtrusionFilter.h>

#include <spinecontoursinterpolation.h>
#include <vtkExtractVOI.h>
#include <vtkAppendPolyData.h>
#include <vtkMath.h>


#include <vnl/vnl_vector.h>

#include <map>

#define PI 3.14159265

using namespace std;

void help(char* execname){
    cout<<"Calculates the thickness surface for the cortex and displays it."<<endl;
    cout<<"Usage: "<<execname<<" -d <patients dir> -p <patient name> [options]"<<endl;
    cout<<"options:"<<endl;
    cout<<"--h --help show help menu"<<endl;
    cout<<"-p <patient name> add another patient";

}


void PolyDataImageStencilExport(vtkPolyData* polydata, vtkImageData *imagein, double *bounds){



    //vtkPolyData* polydata = (vtkPolyData*) poly->GetInput();


    //double *bounds = polydata->GetBounds();
    //cout<<bounds[0]<<" "<<bounds[1]<<" "<<bounds[2]<<" "<<bounds[3]<<" "<<bounds[4]<<" "<<bounds[5]<<endl;

   // vtkSmartPointer<vtkImageData> imagein = vtkSmartPointer<vtkImageData>::New();

    double spacing[3]; // desired volume spacing
    spacing[0] = 0.25;
    spacing[1] = 0.25;
    spacing[2] = 0.25;
    imagein->SetSpacing(spacing);



    int dim[3];
    for (int i = 0; i < 3; i++){
        dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2])/spacing[i])) + 1;
        if (dim[i] < 1){
            dim[i] = 1;
        }
    }
    imagein->SetDimensions(dim);
    imagein->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);
    double origin[3];
    // NOTE: I am not sure whether or not we had to add some offset!
    origin[0] = bounds[0];// + spacing[0] / 2;
    origin[1] = bounds[2];// + spacing[1] / 2;
    origin[2] = bounds[4];// + spacing[2] / 2;
    imagein->SetOrigin(origin);

    imagein->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

    vtkSmartPointer<vtkPolyDataToImageStencil> polytostencil = vtkSmartPointer<vtkPolyDataToImageStencil>::New();

    polytostencil->SetInputData(polydata);
    polytostencil->SetTolerance(0);
    polytostencil->SetOutputOrigin(origin);
    polytostencil->SetOutputSpacing(spacing);
    polytostencil->SetOutputWholeExtent(imagein->GetExtent());

    polytostencil->Update();





    int* extent = imagein->GetExtent();



    for (int x = extent[0]; x <= extent[1]; x++){

        for (int y = extent[2]; y <= extent[3]; y++){

            for (int z  =extent[4]; z <= extent[5]; z++){

                unsigned short* pixel = static_cast<unsigned short*>(imagein->GetScalarPointer(x,y,z));

                *pixel = 0;

            }

        }

    }



    vtkSmartPointer<vtkImageStencil> stencil = vtkSmartPointer<vtkImageStencil>::New();

    stencil->SetInputData(imagein);

    stencil->SetStencilConnection(polytostencil->GetOutputPort());

    stencil->ReverseStencilOn();

    stencil->SetBackgroundValue(128);

    stencil->Update();

    imagein->DeepCopy(stencil->GetOutput());



}


int main(int argv, char *argc[])
{

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

    for(unsigned i = 0; i < contours->GetNumberOfItems(); i++){

        vtkPolyData* nextpoly = contours->GetNextPolyData(it);
        vtkSmartPointer<SPINEContoursInterpolation> interpolation = vtkSmartPointer<SPINEContoursInterpolation>::New();
        interpolation->SetInputData(nextpoly);
        interpolation->Update();
        vtkPolyData* interpolatedcontour = interpolation->GetOutput();
        double* avgnorm = interpolation->GetAvgNormal();

        // prepare the binary image's voxel grid
          vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();
          double bounds[6];
          interpolatedcontour->GetBounds(bounds);
          double spacing[3]; // desired volume spacing
          spacing[0] = 0.25;
          spacing[1] = 0.25;
          spacing[2] = 0.25;
          whiteImage->SetSpacing(spacing);

          // compute dimensions
          int dim[3];
          for (int i = 0; i < 3; i++)
            {
            dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) /
                spacing[i])) + 1;
            if (dim[i] < 1)
              dim[i] = 1;
            }
          whiteImage->SetDimensions(dim);
          whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);
          double origin[3];
          // NOTE: I am not sure whether or not we had to add some offset!
          origin[0] = bounds[0];// + spacing[0] / 2;
          origin[1] = bounds[2];// + spacing[1] / 2;
          origin[2] = bounds[4];// + spacing[2] / 2;
          whiteImage->SetOrigin(origin);
          whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
          // fill the image with foreground voxels:
          unsigned char inval = 255;
          unsigned char outval = 0;
          vtkIdType count = whiteImage->GetNumberOfPoints();
          for (vtkIdType i = 0; i < count; ++i)
            {
            whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
            }

          vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();


          // sweep polygonal data (this is the important thing with contours!)
          vtkSmartPointer<vtkLinearExtrusionFilter> extruder = vtkSmartPointer<vtkLinearExtrusionFilter>::New();

          extruder->SetInputData(interpolatedcontour);
          extruder->SetScaleFactor(1.);
          extruder->SetExtrusionTypeToNormalExtrusion();

          vtkMath::MultiplyScalar(avgnorm, 0.25);
          extruder->SetVector(avgnorm[0], avgnorm[1], avgnorm[2]);
          extruder->Update();

          append->AddInputData(extruder->GetOutput());

          {
              // sweep polygonal data (this is the important thing with contours!)
              vtkSmartPointer<vtkLinearExtrusionFilter> extruder = vtkSmartPointer<vtkLinearExtrusionFilter>::New();

              extruder->SetInputData(interpolatedcontour);
              extruder->SetScaleFactor(1.);
              extruder->SetExtrusionTypeToNormalExtrusion();
              vtkMath::MultiplyScalar(avgnorm, -1);
              extruder->SetVector(avgnorm[0], avgnorm[1], avgnorm[2]);
              extruder->Update();
              append->AddInputData(extruder->GetOutput());
          }

          // polygonal data --> image stencil:
          vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
          pol2stenc->SetTolerance(0); // important if extruder->SetVector(0, 0, 1) !!!
          pol2stenc->SetInputConnection(append->GetOutputPort());
          pol2stenc->SetOutputOrigin(origin);
          pol2stenc->SetOutputSpacing(spacing);
          pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
          pol2stenc->Update();

          // cut the corresponding white image and set the background:
          vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
          imgstenc->SetInputData(whiteImage);
          imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
          imgstenc->ReverseStencilOff();
          imgstenc->SetBackgroundValue(outval);
          imgstenc->Update();

          vtkSmartPointer<vtkMetaImageWriter> imageWriter = vtkSmartPointer<vtkMetaImageWriter>::New();

          char buf[50];
          sprintf(buf, "testImg%d.mhd", i);

          imageWriter->SetFileName(buf);
          imageWriter->SetInputConnection(imgstenc->GetOutputPort());
          imageWriter->Write();

        /*vtkSmartPointer<vtkLinearExtrusionFilter> extruder =
        vtkSmartPointer<vtkLinearExtrusionFilter>::New();
          extruder->SetInputData(interpolatedcontour);
          extruder->SetScaleFactor(1.);
          extruder->SetExtrusionTypeToNormalExtrusion();

          double* norm = interpolation->GetAvgNormal();
          extruder->SetVector(norm[0], norm[1], norm[2]);

          extruder->Update();

        vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>::New();
        PolyDataImageStencilExport(extruder->GetOutput(), img, interpolatedcontour->GetBounds());

        vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
        char buf[50];
        sprintf(buf, "testImg%d.mhd", i);
        writer->SetFileName(buf);
        writer->SetInputData(img);
        writer->Write();*/
    }





    return 0;
}
