#ifndef _GRAPH_H
#define _GRAPH_H

#include <list>
#include "heap.h"

class graph {

 public:
  graph(int capacity);

// Functions for inserting and checking vertices and edges
  bool insertVertex(std::string &id);
  bool checkVertex(std::string &id);
  void insertEdge(std::string &s, std::string &e, int w);

// Performs algorith and writes output to file
  void dijkstra(std::string &s_v);
  void writeOutput(std::string &fn_out);

 private:
 // Vertext needs to be here for edge definition
  class vertex;
 // Edge has direction and weight
  class edge{
      public:
        int weight;
        vertex *v_dest;
  };

 // Vertex with fields necessary for Dijkstra's algorithm
  class vertex{
      public:
        std::string id;
        std::list<edge *> adj;
        bool known;
        int dist;
        vertex *path;
  };

  std::list<vertex *> vertices;
  hashTable *v_map;
};

#endif
