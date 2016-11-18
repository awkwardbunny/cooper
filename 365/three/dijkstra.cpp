// 
// This program allows the user to manipulate a binary heap.
// The program only inserts string ids with associated keys
// into the heap. The heap class, however, is capable of storing
// arbitrary pointers along with each heap item.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

#include "graph.h"

using namespace std;

// Read an integer from standard input; if a non-integer is in the
// buffer, the state is fixed and the user is re-prompted;
// either way, the remainder of the buffer is cleared
void getInteger(string message, int &ref)
{
  bool inputGood = false;
  while (!inputGood) {
    inputGood = true;

    cout << message;
    cin >> ref;

    if (!cin) {
      // Non-integer in input buffer, get out of "fail" state
      cin.clear();
      inputGood = false;
    }
    while (cin.get() != '\n'); // clear buffer
  }
}

int main()
{
    graph g(100);

    // Get user input and open graph file
    string fn_graph;

    cout << "Enter name of graph file: ";
    cin >> fn_graph;

    ifstream f_graph;
    f_graph.open(fn_graph.c_str(), ios::in);

    if(!(f_graph.is_open())){
        cerr << "Error opening the graph file!\n" << endl;
        exit(1);
    }

    // Process the graph file and insert vertices/edges
    string line, s_start, s_end;
    int weight;
    while(getline(f_graph, line)){
//        cout << line << endl; //debugging
        istringstream stream(line);
        stream >> s_start >> s_end >> weight;

        g.insertEdge(s_start, s_end, weight);
    }

    // Get starting vertex from user
    string v_start;

    cout << "Enter a valid vertex id for the starting vertex: ";
    cin >> v_start;

    if(!g.checkVertex(v_start)){
        cout << "Invalid Vertex \"" << v_start << "\"!\n";
        exit(1);
    }

    // Get starting time
    clock_t t_start = clock();
    g.dijkstra(v_start);
    // Get end time
    clock_t t_end = clock();

    // Calculate time difference and print
    //double t_diff = ((double)(t_end-t_start)) / CLOCKS_PER_SEC;
    cout << "Total time (in seconds) to apply Dijkstra's algorithm: " << double((t_end-t_start))/CLOCKS_PER_SEC << endl;

    // Print output to file
    string fn_output;
    cout << "Enter name of output file: ";
    cin >> fn_output;
    g.writeOutput(fn_output);

    return 0;
}
