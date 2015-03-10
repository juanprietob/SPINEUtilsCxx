
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkNormalizeImageFilter.h"

#include "json-c/json.h"

using namespace std;

void help(char* exec){
    cerr<<"Converts a json object image to nifti image format. The image is streamed to the standart output"<<endl;
    cerr<<"The JSON image object structure is : "<<endl;
    cerr<<"{"<<endl;
    cerr<<"spacing : [spcx, spcy, spcz],"<<endl;
    cerr<<"origin : [ox, oy, oz],"<<endl;
    cerr<<"size : [sx, sy, sz],"<<endl;
    cerr<<"matrix : [x0, x1, x2, y0, y1, y2, z0, z1, z2],"<<endl;
    cerr<<"data : [N0, ..., N1]"<<endl;
    cerr<<"}"<<endl;
    cerr<<"Usage: "<<exec<<endl;
    cerr<<"options: "<<endl;
    cerr<<"-help Prints help menu"<<endl;
    cerr<<"-f <json filename>"<<endl;
    cerr<<"-json <string json object>"<<endl;
    cerr<<"-o <outputfilename>"<<endl;
}

void json_parse(json_object * jobj) {
 enum json_type type;
 json_object_object_foreach(jobj, key, val) {
 type = json_object_get_type(val);
 switch (type) {
 case json_type_boolean: printf("type: json_type_boolean, ");
 printf("value: %sn", json_object_get_boolean(val)? "true": "false");
 break;
 }
 }
}

int main( int argc, char ** argv )
{
    string str = "";
    string inputFilename = "";
    string outputFilename = "";

  for(int i = 1; i < argc; i++){
      string param = string(argv[i]);
      if(param.compare("-help") == 0){
          help(argv[0]);
          return EXIT_FAILURE;
      }else if(param.compare("-json") == 0 ){
          str=string(argv[i+1]);
      }else if(param.compare("-f") == 0 ){
          inputFilename=string(argv[i+1]);
      }else if(param.compare("-o") == 0 ){
          outputFilename=string(argv[i+1]);
      }
  }
   bool writeOutputFile = false;
  if(outputFilename.compare("")!=0){
      writeOutputFile = true;
  }

  if(inputFilename.compare("") != 0 && str.compare("") == 0){
      ifstream tempInFile(inputFilename.c_str());
      char c;
      while(tempInFile.get(c)){
          str+=c;
      }
      tempInFile.close();
  }

   if(str.compare("") == 0){
       char c;
       while(cin.get(c)){
           str += c;
       }
   }

   if(str.compare("")==0){
       help(argv[0]);
       return EXIT_FAILURE;
   }

  json_object *new_obj;
  new_obj = json_tokener_parse(str.c_str());

  typedef unsigned short PixelType;
  static const int dimension = 3;

  typedef itk::Image< PixelType, dimension > ImageType;
  ImageType::Pointer img = ImageType::New();

  json_object* dataarray;

  json_object_object_foreach(new_obj, key, val) {
    if(string(key).compare("spacing") == 0){
        int arraylen = json_object_array_length(val);
        int i;
        json_object * jvalue;
        double spacing[3];
        for (i=0; i< arraylen; i++){
            jvalue = json_object_array_get_idx(val, i);
            spacing[i] = json_object_get_double(jvalue);
        }

        img->SetSpacing(spacing);

    }else if(string(key).compare("origin")==0){
        int arraylen = json_object_array_length(val);
        int i;
        json_object * jvalue;
        double origin[3];
        for (i=0; i< arraylen; i++){
            jvalue = json_object_array_get_idx(val, i);
            origin[i] = json_object_get_double(jvalue);
        }

        img->SetOrigin(origin);
    }else if(string(key).compare("size")==0){
        int arraylen = json_object_array_length(val);
        int i;
        json_object * jvalue;
        ImageType::RegionType region;
        ImageType::SizeType size;
        for (i=0; i< arraylen; i++){
            jvalue = json_object_array_get_idx(val, i);
            size[i] = json_object_get_int(jvalue);
        }
        region.SetSize(size);
        img->SetRegions(region);
    }else if(string(key).compare("matrix") == 0){
        int arraylen = json_object_array_length(val);
        int i;
        json_object * jvalue;
        ImageType::DirectionType dir;
        dir.SetIdentity();

        for (i=0; i< arraylen; i++){
            jvalue = json_object_array_get_idx(val, i);
            dir[i%3][(int)i/3] = json_object_get_double(jvalue);
        }
        img->SetDirection(dir);
    }else if(string(key).compare("data") == 0){
        dataarray = val;
    }
  }

  img->Allocate();

  itk::ImageRegionIterator<ImageType> it(img, img->GetLargestPossibleRegion());
  it.GoToBegin();
  int i = 0;
  while(!it.IsAtEnd()){
      json_object *  jvalue = json_object_array_get_idx(dataarray, i);
      int val = json_object_get_int(jvalue);
      it.Set(val);
      i++;
      ++it;
  }

  string filename = "";

  if(writeOutputFile){
      filename = outputFilename;
  }else{
      char buffer[] = "/tmp/SPINEXXXXXXX";
      mktemp(buffer);
      filename = string(buffer);
      filename.append(".nii.gz");
  }

  typedef itk::ImageFileWriter<ImageType> ImageWriter;
  ImageWriter::Pointer writer = ImageWriter::New();
  writer->SetFileName(filename.c_str());
  writer->SetInput(img);
  writer->Update();

  if(!writeOutputFile){
      ifstream tempInFile(filename.c_str());
      char c;
      while(tempInFile.get(c)){
          cout<<c;
      }
      tempInFile.close();

      remove(filename.c_str());
  }

  return EXIT_SUCCESS;
}



