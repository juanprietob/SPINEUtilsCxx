
#include <vtkImageData.h>


void help(char* execname){
    cout<<"Creates an image data and prints the values"<<endl;
    cout<<"Usage: "<<execname<<endl;
}

int main(int argv, char *argc[])
{

    vtkImageData* img = vtkImageData::New();
    img->SetSpacing(1, 1, 1);
    img->SetDimensions(256, 256, 256);
    img->SetOrigin(0, 0, 0);
    img->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

    unsigned short* us = (unsigned short*)img->GetScalarPointer();
    for(int i = 0; i < img->GetNumberOfPoints(); i++){
        cout<<*us;
    }

    return 0;
}

