#include <iostream>
#include "hash.h"
#include <string>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <cstdlib>

using namespace std;

int main() {
    string fn_dict, fn_doc, fn_out;
    ifstream f_dict, f_doc;
    ofstream f_out;

    hashTable *table = new hashTable(1000); //initial size of hashTable

    cout << "Enter name of dictionary: ";
    cin >> fn_dict;
    f_dict.open(fn_dict.c_str(), ios::in);

    if(!(f_dict.is_open())){
        cerr << "Error opening the dictionary file!\n" << endl;
        exit(1);
    }

    clock_t s_time = clock();

    string word;
    while(getline(f_dict, word)){
	    transform(word.begin(), word.end(), word.begin(), ::tolower);   //converts all characters to lowercase
        table->insert(word);
    }

    cout << "Total time (in seconds) to load dictionary: " << double(clock()-s_time)/(double)CLOCKS_PER_SEC << endl;
  
    cout << "Enter name of input file: ";
    cin >> fn_doc;
    f_doc.open(fn_doc.c_str(), ios::in);

    if(!(f_doc.is_open())){
        cerr << "Error opening the document!\n" << endl;
        exit(1);
    }

    cout << "Enter name of output file: ";
    cin >> fn_out;
    f_out.open(fn_out.c_str(), ios::out);

    if(!(f_out.is_open())){
        cerr << "Error opening/creating output file!\n" << endl;
        exit(1);
    }

    s_time = clock();

    string line;
    int line_no = 0;
    int char_no = 0;
    int line_length = 1;
    int word_length = 0;

    bool reported = false; //long and unknown words have been already reported to the user
    bool ignore = false;   //word should be ignored because invalid

    while(getline(f_doc, line)) {
        word.clear();

        line_no++;
        char_no = 0;
        word_length = 0;
        line_length = line.length();
        
        if(line_length == 0){
            continue;
        }

        while(char_no <= line_length){
            if(isalnum(line[char_no]) || line[char_no] == '-' || line[char_no] == '\''){ //if a valid word character
                
	            if(isdigit(line[char_no])){     //ignore words with numbers in them
	                ignore = true;
	            }
	            
                if(++word_length <= 20){
	                word += line[char_no];
	            }else{
	                if(!reported){
	                    transform(word.begin(), word.end(), word.begin(), ::tolower);
	                    f_out << "Long word at line " << line_no << ", starts: " << word << endl;
	                    reported = true;
	                }
	                ignore = true;
	            }
            }else{              //invalid word characters
	            if(ignore){
	                ignore = false;
	                reported = false;
                }else{
	                transform(word.begin(), word.end(), word.begin(), ::tolower);
	                if(word_length && !(table->contains(word))){                        //if no such word exist in table (and is not empty)
	                    transform(word.begin(), word.end(), word.begin(), ::tolower);
	                    f_out << "Unknown word at line " << line_no << ": " << word << endl;
	                }
	            }

	            word.clear();
	            word_length = 0;
            }
	        char_no++;
        }

    }
    
    cout << "Total time (in seconds) to check document: " << double(clock()-s_time)/(double)CLOCKS_PER_SEC << endl;
}
