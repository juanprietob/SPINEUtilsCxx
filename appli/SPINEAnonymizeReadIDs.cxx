

#include <iostream>


#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <map>

using namespace std;


int main(int argc, char *argv[])
{
  if( argc < 1 )
    {
        std::cerr << argv[0] << " fileNameIds " << std::endl;
        return 1;
    }


    vector< string> elems;
    std::ifstream  data(argv[1]);

    std::string line;
    typedef map<int, int> mapids;
    mapids ids;
    while(std::getline(data,line, '\r'))
    {
        std::stringstream  lineStream(line);
        std::string        cell;
        int i = 0;
        int patientId, mrn;
        while(std::getline(lineStream,cell,','))
        {
            if(i == 0){
                patientId = atoi(cell.c_str());
            }else if(i == 1){
                mrn = atoi(cell.c_str());
            }
            i++;
            elems.push_back(cell);
        }
        ids.insert(pair<int, int>(patientId, mrn));
    }
    mapids::iterator it;

    for(it = ids.begin(); it != ids.end(); ++it){
        int patientId = (*it).first;
        int mrn = (*it).second;

        cout<<patientId<<", "<<mrn<<endl;
        string exec = "for files in $(find . -name *.xml -type f); do grep -l $ids $files | while read xml; do ${xml%/*}; done; done;";

    }
    return 0;
}


