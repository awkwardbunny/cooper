#ifndef _GRAPH_H
#define _GRAPH_H

#include <list>
#include "heap.h"

class graph {

 public:
  graph(int capacity);

  bool insertVertex(std::string &id);
  bool checkVertex(std::string &id);
  void insertEdge(std::string &s, std::string &e, int w);

  void dijkstra(std::string &s_v);
  void writeOutput(std::string &fn_out);

 private:
  class vertex;
  class edge{
      public:
        int weight;
        vertex *v_dest;
  };

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
