
/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library

  Copyright (c) 2006-2011 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*
 * Dummy implementation of C.7.1.3 Clinical Trial Subject Module
 *
 * Usage:
 *  ClinicalTrialAnnotateTest gdcmData/012345.002.050.dcm out.dcm
 */

#include "gdcmReader.h"
#include "gdcmWriter.h"
#include "gdcmAnonymizer.h"

#include "gdcmUIDGenerator.h"
#include "gdcmFile.h"
#include "gdcmTag.h"
#include "gdcmSystem.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
  if( argc < 4 )
    {
    std::cerr << argv[0] << " input.dcm outdir studyId sessionId" << std::endl;
    return 1;
    }
  string filename(argv[1]);
  string session = "";
  string projectId = "NPMSC_MGHB";
  string subjectId = "subject1";

  if(argc > 4){
      session = argv[4];
  }
  if(filename.find("(")!=string::npos){
      filename.replace(filename.find("("), filename.find("(") + 1, "\(");
  }
  if(filename.find(")")!=string::npos){
      filename.replace(filename.find(")"), filename.find(")") + 1, "\)");
  }

  string imagedir(filename) ;
  imagedir = imagedir.substr(0, imagedir.find_last_of("/"));
  imagedir = imagedir.substr(imagedir.find_last_of("/") + 1);

  cout<<"input= "<< filename<<endl;

  string outfilename = string(argv[2]);
  string outdir = outfilename.substr(0, outfilename.find_last_of("/"));

  string exec = "mkdir -p " + outdir;
  system (exec.c_str());
  /*exec = "mkdir -p " + string(argv[2]) + "/"  + imagedir;
  system (exec.c_str());*/


  gdcm::Reader reader;
  reader.SetFileName( filename.c_str() );
  if( !reader.Read() )
    {
    std::cerr << "Could not read: " << filename << std::endl;
    return 1;
    }

  // The output of gdcm::Reader is a gdcm::File
  /*gdcm::File &file = reader.GetFile();

  // the dataset is the the set of element we are interested in:
  gdcm::DataSet &ds = file.GetDataSet();
  //gdcm::Tag tsis(0x0008,0x2112); // SourceImageSequence
  gdcm::Tag tbday(0x0010,0x0030);
  gdcm::DataElement bday;
  if ( ds.FindDataElement( tbday ) ){
       bday.SetValue(ds.GetDataElement( tbday ).GetValue());
  }*/

  gdcm::Anonymizer ano;
  ano.SetFile( reader.GetFile() );

  ano.RemoveGroupLength();
  ano.RemovePrivateTags();

  gdcm::MediaStorage ms = gdcm::MediaStorage::SecondaryCaptureImageStorage;

    gdcm::UIDGenerator gen;
    ano.Replace( gdcm::Tag(0x0008,0x16), ms.GetString() );
    //std::cout << ms.GetString() << std::endl;
    ano.Replace( gdcm::Tag(0x0008,0x18), gen.Generate() );
    //
    ano.Replace( gdcm::Tag(0x0010,0x20), argv[3] );
    ano.Replace( gdcm::Tag(0x0010,0x10), "Anonim" );
    ano.Replace( gdcm::Tag(0x0002,0x0010), "1.2.840.10008.1.2.1" );//Compression

    ano.Empty( gdcm::Tag(0x0010,0x30) );
    ano.Empty( gdcm::Tag(0x0010,0x40) );
    ano.Empty( gdcm::Tag(0x0008,0x20) );
    ano.Empty( gdcm::Tag(0x0008,0x30) );
    ano.Empty( gdcm::Tag(0x0008,0x90) );
    ano.Empty( gdcm::Tag(0x0020,0x10) );
    ano.Empty( gdcm::Tag(0x0020,0x11) );
    ano.Empty( gdcm::Tag(0x0008,0x50) );
    ano.Empty( gdcm::Tag(0x0008,0x50) );
    ano.Empty( gdcm::Tag(0x0032,0x4000) );




    gdcm::StringFilter sf;
    sf.SetFile(reader.GetFile());
    string strtag = sf.ToString(gdcm::Tag(0x0008, 0x103e));


    if(strtag.find("screen save") != string::npos || strtag.find("Screen Save") != string::npos){

        cout<<"Screen capture, image not saved. "<<endl;

    }else{
        /*ano.Empty( gdcm::Tag(0x0020,0x0013) );
        ano.Replace( gdcm::Tag(0x0020,0xd), gen.Generate() );
        ano.Replace( gdcm::Tag(0x0020,0xe), gen.Generate() );
        ano.Replace( gdcm::Tag(0x0008,0x64), "WSD " );*/


        cout<<"output anonymized: "<< outfilename<<endl;
      gdcm::Writer writer;
      writer.SetFile( reader.GetFile() );
      writer.SetFileName( outfilename.c_str() );

      if(session.compare("")!=0){

          while(strtag.find(" ")!=string::npos){
              strtag = strtag.replace(strtag.find(" "), 1, "");
          }

          /*string exec = "XNATRestClient -host https://xnat.utahdcc.org/DCCxnat -u jprieto -p UncK-9Doggy -m PUT -remote \"/data/archive/projects/NPMSC_MGHB/subjects/TEST_MGHB1/experiments/"+session+"?xnat:mrSessionData/date=12/07/14\"";
          cout<<exec<<endl<<endl;
          system(exec.c_str());

          exec = "XNATRestClient -host https://xnat.utahdcc.org/DCCxnat -u jprieto -p UncK-9Doggy -m PUT -remote \"/data/archive/projects/NPMSC_MGHB/subjects/TEST_MGHB1/experiments/"+session+"/scans/"+strtag+"?xsiType=xnat:mrScanData&xnat:mrScanData/type="+strtag+"\"";
          cout<<exec<<endl<<endl;
          system(exec.c_str());

          exec = "XNATRestClient -host https://xnat.utahdcc.org/DCCxnat -u jprieto -p UncK-9Doggy -m PUT -remote \"/data/archive/projects/NPMSC_MGHB/subjects/TEST_MGHB1/experiments/"+session+"/scans/"+strtag+"/resources/DICOM/files/\" -local "+outfilename;
          cout<<exec<<endl<<endl;
          system(exec.c_str());*/


          //exec = "curl -H 'Content-Type: application/dicom' -X POST -u jprieto:UncK-9Doggy \"https://xnat.utahdcc.org/DCCxnat/data/services/import?inbody=true&PROJECT_ID="+projectId+"&SUBJECT_ID="+subjectId+"&EXPT_LABEL="+session+"\" --data-binary @"+outfilename;
          //cout<<exec<<endl<<endl;
          //system(exec.c_str());
      }




      if( !writer.Write() )
        {
        return 1;
        }

    }

  return 0;
}

