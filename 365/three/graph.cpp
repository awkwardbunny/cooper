#include <iostream>
#include <fstream>
#include <limits.h>
#include "graph.h"

#define INFINITY INT_MAX

using namespace std;

/** Tried to keep consistent variable names of format x_yyyy, where x is the variable type and yyyy is the variable description.
    ex: i_edge is an interator of edge pointers
        v_dest is a vertex destination
**/


// Create the graph using hashTable
graph::graph(int capacity = 100){
    v_map = new hashTable(capacity);
}

// Inserts an edge between vertices 's' and 'e' of weight 'w'
// If vertice(s) don't exist, create them using insertVertex() function
void graph::insertEdge(string &s, std::string &e, int w){
    insertVertex(s);
    insertVertex(e);
    
    bool key_exists = false;  // Isn't really used for anything but to match the getPointer function call. We also know that they exist from above lines.
    vertex *v_dest = static_cast<vertex *>(v_map->getPointer(e, &key_exists));

    // Create the edge
    edge *link = new edge();
    link->weight = w;
    link->v_dest = v_dest;

    vertex *v_source = static_cast<vertex *>(v_map->getPointer(s, &key_exists));
    v_source->adj.push_back(link);

    return;
}

// Checks whether the vertex exists in the map
bool graph::checkVertex(string &id){
    return v_map->contains(id);
}

// Inserts vertex into map and the vertices list
bool graph::insertVertex(string &id){
    if(checkVertex(id)) return false;

    vertex *v = new vertex();
    v->id = id;
    
    v_map->insert(id, v);
    vertices.push_back(v);
    return true;
}

// Prints the result to the file specified by filename fn_out
void graph::writeOutput(string &fn_out){
    ofstream f_output;
    f_output.open(fn_out.c_str(), ios::out);

    list<vertex *>::iterator i;
    string path;

    vertex *v_i;
    for(i = vertices.begin(); i != vertices.end(); i++){
        v_i = *i;
        f_output << v_i->id << ": "; // << (v_i->dist == INIFINITY) ? "NO PATH" : v_i->dist; // Commented out section integrated below

        if(v_i->dist == INFINITY)
            f_output << "NO PATH" << endl;
        else{
            // Follow the path pointers backwards to get reverse path.
            path = v_i->id;
            while(v_i->path){
                v_i = v_i->path;
                path = v_i->id + ", " + path; // Prepend the current id to the path to un-reverse
            }

            f_output << v_i->dist << " [" << path << "]" << endl;
        }
    }
}

void graph::dijkstra(string &s_v){
    // Heap to hold all the vertices
    heap edsger(vertices.size() + 1);

    // Setup; intialize all values for algorithm
    std::list<vertex *>::iterator i;
    for(i = vertices.begin(); i != vertices.end(); i++){
        vertex *v_i = *i;
        v_i->dist = (v_i->id == s_v) ? 0 : INFINITY;

        v_i->known = false;
        v_i->path = NULL;

        edsger.insert(v_i->id, v_i->dist, v_i);
    }
    
    // Perform Dijkstra's Algorithm
    vertex *v;
    while(edsger.deleteMin(NULL, NULL, &v) != 1){
        v->known = true;
        if(v->dist == INFINITY){
            continue;
        }

        list<edge *>::iterator i_edge;
        for(i_edge = v->adj.begin(); i_edge != v->adj.end(); i_edge++){
            edge *e_i = *i_edge;
            int w = v->dist + e_i->weight;
            if(w < e_i->v_dest->dist){
                e_i->v_dest->dist = w;
                edsger.setKey(e_i->v_dest->id, w);
                e_i->v_dest->path = v;
            }
        }
    }

    return;
}
