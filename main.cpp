#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sys/stat.h>

using namespace std;

class ExternalSorter {

public:
    /* Constructor is passed input file name and maximum # of rows in temp files */

    ExternalSorter(string inputFileName  , size_t maxFileSize = 4) {
        inFile.open(inputFileName.c_str());

        if (!inFile)  {
            cerr <<" Error opening input file!\n";
        }
        this->maxFileSize = maxFileSize;
    }

    void Sort() {

        size_t nFiles = splitToSortedFiles();
        if (nFiles <=0) {
            cerr <<"Error splitting!\n";
            return;
        }
       // cout <<nFiles<<endl;
        vector<size_t> filesPositions (nFiles + 1 , 1);
        ofstream out ("sorted_data.txt");
        if(!out){
            cerr << "Error creating sorted file\n";
            return;
        }

        while (true) {

            string minRow;
            size_t minFileNum = -1;
            bool atleastOneFileRemaining = false;
        for (size_t i=1;i<=nFiles;i++) {

             size_t curRowPos = filesPositions[i];
             if (curRowPos ==-1) //finished file
                 break;

            string fileName = to_string(i) +".dat";
            ifstream in("temp//" + fileName);
            if(!in) {
                cerr <<"Error opening temp files!\n";
                return;
            }

            string curRow = getLineNum(in , curRowPos);
            if (curRow==""){ //mark file finished
                filesPositions[i] = -1;
                break;
            }
           // cout <<curRow<<endl;
            atleastOneFileRemaining = true;
            if (minRow =="" || curRow < minRow)
            {
                minFileNum = i;
                minRow = curRow;

            }


            in.close();



        }
        if (!atleastOneFileRemaining)
            break;

        filesPositions[minFileNum]++;
        out << minRow <<"\n"; //output min item to sorted file
         //increment pos at file with min item


        }


    }

    string getLineNum(ifstream &in , size_t num){
        string s;
        for (size_t i=1;i<=num;i++){
            getline(in,s);
            if (!in)
                return "";
        }
        return s;

    }

   private:
    ifstream inFile;
    size_t maxFileSize;

    size_t splitToSortedFiles () {

        /* creates temp sorted file each with maxFileSize # of rows
           returns number of box files
        */

        //create directory to store temp files
        mkdir("temp",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        size_t curFile = 0;
        string row;
        do {
            vector<string> fileData;
            //take data
            size_t cnt=0; //# of rows taken
            while (inFile && cnt<maxFileSize){
                getline(inFile,row);
                if(row.empty())//check for empty row
                    break;
                fileData.push_back(row);
                cnt++;
            }
            if (cnt ==0) // nothing read
                break;


            //sort data;
            sort(fileData.begin(),fileData.end());

            //Create file
            ofstream out;
            curFile++;
            string fileName = "temp//" + to_string(curFile) + ".dat";
            out.open(fileName);

            //output to cur box file
            for (size_t i=0;i<cnt;i++)
                out <<fileData[i]<<"\n";

            //close box file
            out.close();
        }while (inFile);


        return curFile;

    }




};



int main(){

    ExternalSorter es("data.txt",7);
   es.Sort();
}
