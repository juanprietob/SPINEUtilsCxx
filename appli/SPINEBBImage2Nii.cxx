
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"

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
    cerr<<"-sep <add a separation string to use multiple images>"<<endl;
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
    string sep = "";

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
      }else if(param.compare("-sep") == 0){
          sep = string(argv[i+1]);
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

   bool isTempFile = false;
   string filename = "";

   string::size_type pos = str.find(sep);
   if(pos != string::npos && sep.length() > 0){
       string img = str.substr(0, pos);

       char buffer[] = "/tmp/SPINEXXXXXXX";
       mktemp(buffer);
       filename = string(buffer);
       filename.append(".nii.gz");

       ofstream tempFile;
       tempFile.open (filename.c_str());
       tempFile << img;
       tempFile.close();
       isTempFile = true;

       str = str.substr(pos + sep.length());//img data
   }

  json_object *new_obj;
  new_obj = json_tokener_parse(str.c_str());
  str = "";

  typedef unsigned short PixelType;
  static const int dimension = 3;

  typedef itk::Image< PixelType, dimension > ImageType;
  ImageType::RegionType region;
  ImageType::SizeType size;
  ImageType::DirectionType dir;
  dir.SetIdentity();
  ImageType::Pointer img = 0;

  if(isTempFile){
      typedef itk::ImageFileReader<ImageType> ReaderType;
      ReaderType::Pointer read = ReaderType::New();
      read->SetFileName(filename.c_str());
      read->Update();

      img = read->GetOutput();

  }else{
      img = ImageType::New();
  }



  json_object* dataarray;

  json_object_object_foreach(new_obj, key, val) {
    if(string(key).compare("spacing") == 0){
        double spacing[3];
        spacing[0] = 1;
        spacing[1] = 1;
        spacing[2] = 1;

        int arraylen = json_object_array_length(val);
        int i;
        json_object * jvalue;

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
        origin[0] = 0;
        origin[1] = 0;
        origin[2] = 0;

        for (i=0; i< arraylen; i++){
            jvalue = json_object_array_get_idx(val, i);
            origin[i] = json_object_get_double(jvalue);
        }

        img->SetOrigin(origin);

    }else if(string(key).compare("size")==0){
        int arraylen = json_object_array_length(val);
        int i;
        json_object * jvalue;

        for (i=0; i< arraylen; i++){
            jvalue = json_object_array_get_idx(val, i);
            size[i] = json_object_get_int(jvalue);
        }
        region.SetSize(size);

        img->SetRegions(region);
    }else if(string(key).compare("matrix") == 0){

        int i, j;
        json_object * jvalue, *arr;

        for (i=0; i< dimension; i++){
            arr = json_object_array_get_idx(val, i);
            for (j=0; j < dimension; j++){
                jvalue = json_object_array_get_idx(arr, j);
                dir[i][j] =  json_object_get_double(jvalue);
            }
        }

        img->SetDirection(dir);

    }else if(string(key).compare("data") == 0){
        dataarray = val;
    }
  }


  if(!isTempFile){
      try{
          img->Allocate();
      }catch(std::exception e){
          cerr<<e.what()<<endl;
          return EXIT_FAILURE;
      }
  }

  itk::ImageRegionIterator<ImageType> it(img, img->GetLargestPossibleRegion());
  it.GoToBegin();
  int i = 0;
  while(!it.IsAtEnd()){
      json_object *  jvalue = json_object_array_get_idx(dataarray, i);
      double val = json_object_get_double(jvalue);
      it.Set((PixelType)val);
      i++;
      ++it;
  }

  if(writeOutputFile){
      filename = outputFilename;
  }else if(!isTempFile){
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
  }

  if(!writeOutputFile || isTempFile){
      remove(filename.c_str());
  }

  return EXIT_SUCCESS;
}



