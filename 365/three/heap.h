#ifndef _HEAP_H
#define _HEAP_H

#include <vector>
#include <string>
#include "hash.h"

class heap {

 public:
  heap(int capacity);

  int insert(const std::string &id, int key, void *pv = NULL);
  int setKey(const std::string &id, int key);
  int deleteMin(std::string *pId = NULL, int *pKey = NULL, void *ppData = NULL);
  int remove(const std::string &id, int *pKey = NULL, void *ppData = NULL);

 private:
  class node {
  public:
    std::string id;
    int key;
    void *pData;
  };

  int capacity;
  int filled;

  std::vector<node> data;
  hashTable *mapping;

  void percolateUp(int posCur);
  void percolateDown(int posCur);
  int getPos(node *pn);
  
};

#endif
