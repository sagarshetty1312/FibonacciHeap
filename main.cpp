#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "MaxFibHeap.h"

using namespace std;

const string WHITESPACE = " \n\r\t\v\f";

//Function to remove leading and trailing whitespaces in the string.
string trim(string str){
    
    if(str.size() == 0) return "";

    size_t found = str.find_first_not_of(WHITESPACE);//Check for leading whitespaces
    if(found == 4294967295) return "";//Checking for strings with only whitespaces.
    str = (found == str.size()) ? str : str.substr(found);

    found = str.find_last_not_of(WHITESPACE);//Check for trailing whitespaces
    str = (found == str.size()-1) ? str : str.substr(0,found+1);
    return str;
}

//Function to extract frequency and trim hashtag for entry of the type '#hashtag 45'
int extractFreqandTrim(string* strPtr){
    string& str = *strPtr;
    size_t index = str.find(" ",1);
    int frequency = stoi(str.substr(index+1));
    str = str.substr(1,index-1);
    return frequency;
}

void endProgram(){
    exit(0);
}

//Processes string based on the type of input and returns string if input is a query
string processString(string str, unordered_map<string, Node*>* tablePtr, MaxFibHeap* maxFibHeapPtr){
    unordered_map<string, Node*>& hashTable = *tablePtr;
    MaxFibHeap& maxFibHeap = *maxFibHeapPtr;
    string result = "";
    str = trim(str);

    if(str[0] == '#'){
        //Input type hashtag
        int frequency = extractFreqandTrim(&str);
        if(str.size() == 0) return result;
        
        if(hashTable.find(str) == hashTable.end())
        {//insert for first occurrence of hashtag
            Node* nodePtr = maxFibHeap.insertNewNode(frequency);
            hashTable[str] = nodePtr;
            
        } 
        else 
        {//increase key operation
            Node& node = *hashTable[str];
            maxFibHeap.increaseFrequency(&node, frequency + node.frequency);
        }

    } else if(((str.compare("stop")) == 0) || ((str.compare("STOP")) == 0)) {
        //do nothing if "stop"
    } else 
    {
        //For query(number) as an input
        int count = stoi(str);
        int countBuffer = 0;
        Node* nodes[count];//Array of extracted nodes
        string strings[count];//Array of respective hashtags
        string comma = "";
        if(count == 0) return result;

        while (countBuffer < count)
        {//Fetching the max node from the heap and storing
            Node* maxNodePtr = maxFibHeap.removeMax();
            Node& maxNode = *maxNodePtr;
            nodes[countBuffer] = maxNodePtr;
            countBuffer++;
        }

        countBuffer = 0;
        while(countBuffer < count)
        {//Fetching the respective hashtag for the extracted nodes and storing
            unordered_map<string, Node*>:: iterator itr; 
            for(itr = hashTable.begin() ; itr != hashTable.end(); itr++)
            {
                if(itr-> second == nodes[countBuffer]){
                    strings[countBuffer] = itr->first;
                    break;
                }
            }
            countBuffer++;
        }


        countBuffer = 0;
        while (countBuffer < count)
        {//Appending response to result string and reinserting nodes to heap
            result.append(comma);
            result.append(strings[countBuffer]);
            comma = ",";
            maxFibHeap.insertIntoTopLevel(nodes[countBuffer]);
            countBuffer++;
        }
    }
    return result;
}

//Main function
int main(int argc, char** argv){
    unordered_map<string, Node*> hashTable;
    static MaxFibHeap mfibHeap = MaxFibHeap();
    string str;
    string outstr = "";
    string cr = "";
    ifstream ifile(argv[1]);
    while(str.compare("stop") != 0){
        getline(ifile,str);
        string result = processString(str, &hashTable, &mfibHeap);
        if((result.compare("")) != 0){
            outstr.append(cr);
            outstr.append(result);
            cr = "\n";
        }
    }
    ifile.close();
    if(argc == 2){
        cout << outstr << endl;
    } else {
        ofstream ofile;
        ofile.open(argv[2]);
        ofile << outstr;
        ofile.close();
    }
    
}