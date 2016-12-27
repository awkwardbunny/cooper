/*********************************
 * Brian (Seok hyeon) Hong
 * DSA II Assignment #4 * Professor Sable * 
 * Dynamic programming to check
 * whether given is a merge of
 * two other words.
 ********************************/
#include<stdio.h>
#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<stack>
#include<string.h>

using namespace std;

int matrix[1001][1001];

int main(){

    // Handle files
    string inputFile, outputFile;
    cout << "Enter name of input file: ";
    cin >> inputFile;

    ifstream f_in(inputFile.c_str());
    if(!f_in.is_open()){
        cerr << "Error: Could not open " << inputFile << " for reading\n";
        return 1;
    }

    cout << "Enter name of output file: ";
    cin >> outputFile;

    ofstream f_out(outputFile.c_str());
    if(!f_out.is_open()){
        cerr << "Error: Could not open " << outputFile << " for writing\n";
        return 1;
    }

    // Detect whether is a merge or not
    string s_a, s_b, s_c; // for strings A, B, and C (merged)
    int n_a, n_b, n_c;

    int n = 1;
    while(n && !f_in.eof()){
        f_in >> s_a >> s_b >> s_c;
        
        if(f_in.eof())
            break;

        n_a = s_a.length();
        n_b = s_b.length();
        n_c = s_c.length();

        //cout << "A:" << s_a << "\nB:" << s_b << "\nC:" << s_c << endl;
        
        // an easy preliminary check
        if(n_c != n_a + n_b){
            f_out << "*** NOT A MERGE ***" << endl;
            continue;
        }

        memset(matrix, 0, 1001*1001);
        matrix[0][0] = 1;

        for(int j = 0; j <= n_a; j++){
            for(int i = 0; i <= n_b; i++){
                /*
                if(i == 0 && j == 0)
                    continue;
                
                if(i == 0){
                    if(s_a[j-1] == s_c[j-1])
                        matrix[i][j] = matrix[i][j-1];
                }else if(j == 0){
                    if(s_b[i-1] == s_c[i-1])
                        matrix[i][j] = matrix[i-1][j];
                }else if(s_b[i-1] == s_c[i+j-1] && s_a[j-1] != s_c[i+j-1]){
                    matrix[i][j] = matrix[i-1][j];
                }else if(s_a[j-1] == s_c[i+j-1] && s_b[i-1] != s_c[i+j-1]){
                    matrix[i][j] = matrix[i][j-1];
                }else if(s_a[j-1] == s_c[i+j-1] && s_b[i-1] == s_c[i+j-1]){
                    matrix[i][j] = (matrix[i][j-1] || matrix[i-1][j]);
                }
                */

                if(matrix[i][j]){
                    if(s_a[j] == s_c[i+j] && !matrix[i][j+1] && j < 1000){
                        matrix[i][j+1] = 1;
                    }
                    if(s_b[i] == s_c[i+j] && i < 1000){
                        matrix[i+1][j] = 1;
                    }
                }
            }
        }
        
        /*
        // Test code to print out the matrix
        for(int y = 0; y < n_c; y++){
            for(int x = 0; x < n_c; x++){
                printf("%d ", matrix[x][y]);
            }
            cout << endl;
        }
        */

        if(!matrix[n_b][n_a]){
            f_out << "*** NOT A MERGE ***" << endl;
            continue;
        }

        //f_out << s_c << endl; // prints out the merged word (without uppercase)

        stack<char> stack_output;

        // Build the output string backwards as we find our path back up
        int i = n_b;
        int j = n_a;
        int bleh = 0;
        stack_output.push('\0');
        while(i || j){
            if(i && j){
                // Check to the left (i-1) first to give priority to string A
                //(if both path are possible)
                if(matrix[i-1][j])
                    stack_output.push(s_b[--i]);
                else if(matrix[i][j-1])
                    stack_output.push(toupper(s_a[--j]));
                else{
                    cout << "Unreacheable statement, but like last time, I may be wrong... ERROR!" << endl;
                    bleh = 1;
                    break;
                }
            }else if(j){
                if(matrix[i][j-1])
                    stack_output.push(toupper(s_a[--j]));
                else{
                    cout << "Unreacheable statement(1), but like last time, I may be wrong... ERROR!" << endl;
                    bleh = 1;
                    break;
                }
            }else{
                if(matrix[i-1][j])
                    stack_output.push(s_b[--i]);
                else{
                    cout << "Unreacheable statement(2), but like last time, I may be wrong... ERROR!" << endl;
                    bleh = 1;
                    break;
                }
            }
        }

        if(bleh){
            cout << "Also should not be running" << endl;
            f_out << "*** NOT A MERGE ***" << endl;
            continue;
        }
        
        stringstream ss_out;
        // Spit it back out
        while(!stack_output.empty()){
            ss_out << stack_output.top();
            stack_output.pop();
        }

        f_out << ss_out.str() << endl;
        //n = 0; // for debugging (while loops one iteration only)
    }
}
