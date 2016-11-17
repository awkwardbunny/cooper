#include <iostream>
#include <fstream>
#include <limits.h>
#include "graph.h"

#define INFINITY INT_MAX

using namespace std;

graph::graph(int capacity = 100){
    v_map = new hashTable(capacity);
}

void graph::insertEdge(string &s, std::string &e, int w){
    insertVertex(s);
    insertVertex(e);
    
    bool key_exists = false;  // Isn't really used for anything but to match the getPointer function call.
    vertex *v_dest = static_cast<vertex *>(v_map->getPointer(e, &key_exists));

    edge *link = new edge();
    link->weight = w;
    link->v_dest = v_dest;

    vertex *v_source = static_cast<vertex *>(v_map->getPointer(s, &key_exists));
    v_source->adj.push_back(link);

    return;
}

bool graph::checkVertex(string &id){
    return v_map->contains(id);
}

bool graph::insertVertex(string &id){
    if(checkVertex(id)) return false;

    vertex *v = new vertex();
    v->id = id;
    
    v_map->insert(id, v);
    vertices.push_back(v);
    return true;
}

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
            path = v_i->id;
            while(v_i->path){
                v_i = v_i->path;
                path = v_i->id + ", " + path;
            }

            f_output << v_i->dist << " [" << path << "]" << endl;
        }
    }
}

void graph::dijkstra(string &s_v){
    heap edsger(vertices.size() + 1);

    std::list<vertex *>::iterator i;
    for(i = vertices.begin(); i != vertices.end(); i++){
        vertex *v_i = *i;
        v_i->dist = (v_i->id == s_v) ? 0 : INFINITY;

        v_i->known = false;
        v_i->path = NULL;

        edsger.insert(v_i->id, v_i->dist, v_i);
    }
    


    return;
}
