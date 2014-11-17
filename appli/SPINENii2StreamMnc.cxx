
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"


using namespace std;

void help(char* exec){
    cerr<<"Converts a nifti image to BrainBrowser format. The input image must be provided by the standart input,"<<endl;
    cerr<<"ex. cat <imageFilename> | "<<string(exec)<<" -h"<<endl;
    cerr<<"Usage: "<<exec<<endl;
    cerr<<"options: "<<endl;
    cerr<<"-h   Prints the header information in BB format to the standart output"<<endl;
    cerr<<"-c   Prints the content of the file to the standart output"<<endl;
}

int main( int argc, char ** argv )
{

  string filename = "";
  bool printHeader = false;
  bool printContent = false;

  //string imagestring = "";
  char c;
  string str = "";
  while(cin.get(c)){
      str += c;
  }

  for(int i = 1; i < argc; i++){
      string param = string(argv[i]);
      if(param.compare("-f") == 0){
          filename = string(argv[i+1]);
      }else if(param.compare("-h") == 0){
          printHeader = true;
      }else if(param.compare("-c") == 0){
          printContent = true;
      }
  }

  if(str.compare("") == 0 || (!printHeader && !printContent)){
      help(argv[0]);
      return EXIT_FAILURE;
  }

  typedef unsigned short PixelType;
  static const int dimension = 3;

  char buffer[] = "/tmp/SPINEXXXXXXX";
  mktemp(buffer);
  filename = string(buffer);
  filename.append(".nii.gz");

  ofstream tempFile;
  tempFile.open (filename.c_str());
  tempFile << str;
  tempFile.close();

  typedef itk::Image< PixelType, dimension > ImageType;
  typedef  itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename.c_str() );
  reader->Update();

  ImageType::Pointer image = 0;

  try{
        image = reader->GetOutput();
  }catch (itk::ExceptionObject &excp)
  {
  cerr << "Exception thrown while reading the image. " << filename <<endl;
  cerr << excp << endl;
  return EXIT_FAILURE;
  }

  if(printHeader){

      ImageType::DirectionType direction = image->GetDirection();
      ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
      ImageType::PointType origin = image->GetOrigin();
      ImageType::SpacingType spacing = image->GetSpacing();

      vector<string> order;
      order.push_back("xspace");
      order.push_back("yspace");
      order.push_back("zspace");

      cout<<"{";
      cout<<"\"order\" : [";
      for(unsigned i = 0; i < order.size(); i++){
          cout<<"\"" + order[i] + "\"";
          if(i < order.size() - 1){
              cout<<",";
          }
      }
      cout<<"],";

      for(unsigned i = 0; i < order.size(); i++){
          cout<<"\"" + order[i] + "\" : {";
          cout<<"\"direction_cosines\" : [";
          for(unsigned j = 0; j < 3; j++){
              cout<<direction[i][j];
              if(j < 2){
                  cout<<",";
              }
          }
          cout<<"],";
          cout<<"\"space_length\" : "<<size[i]<<",";
          cout<<"\"start\" : "<<origin[i]<<",";
          cout<<"\"step\" : " <<spacing[i];
          cout<<"}";
          if(i < order.size() - 1){
              cout<<",";
          }
      }
      cout<<"}"<<endl;
  }

  if(printContent){
      typedef itk::ImageRegionIterator<ImageType> IteratorType;
      IteratorType it(image, image->GetLargestPossibleRegion());
      it.GoToBegin();
      int numt = sizeof(PixelType);
      char *v = new char[numt];
      while(!it.IsAtEnd()){
          PixelType p = it.Get();
          char* vpoint = (char*) &p;
          for(int i = 0; i < numt; i++){
              v[i] = *vpoint;
              vpoint++;
          }
          cout.write(const_cast<char*>(v), numt);
          ++it;
      }
      delete v;
  }
  remove(filename.c_str());
  return EXIT_SUCCESS;
}


