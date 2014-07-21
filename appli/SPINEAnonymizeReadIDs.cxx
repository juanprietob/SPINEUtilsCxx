
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

#include <stdlib.h> 

using namespace std;

void help(char* exec){
    cerr<<"usage: "<<exec<<" -dcm <dicom file name>"<<endl;
    cerr<<"output: get the patientID = sessionId for XNAT"<<endl;
}

int main(int argv, char *argc[])
{


    string dcmFile = "";

    for(int i = 1; i < argv; i++){
        string input = argc[i];
        if(input.compare("-dcm")==0){
            i++;
            dcmFile = argc[i];
        }else if(input.compare("-help")==0 || input.compare("--help")==0){
            help(argc[0]);
            return 0;
        }

    }


  string filename = dcmFile;

  if(filename.find("(")!=string::npos){
      filename.replace(filename.find("("), filename.find("(") + 1, "\(");
  }
  if(filename.find(")")!=string::npos){
      filename.replace(filename.find(")"), filename.find(")") + 1, "\)");
  }

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


    gdcm::StringFilter sf;
    sf.SetFile(reader.GetFile());
    string sessionId = sf.ToString(gdcm::Tag(0x0010,0x20));

    cout<<sessionId<<endl;

  return 0;
}

