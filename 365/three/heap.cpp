#include <iostream>
#include "heap.h"

using namespace std;

heap::heap(int capacity){
    this->capacity = capacity;
    this->filled = 0;

    data.resize(capacity+1);
    mapping = new hashTable(capacity*2);
}

int heap::insert(const std::string &id, int key, void *pv){
    if(filled == capacity) return 1;
    if(mapping->contains(id)) return 2; 

    filled++;
    data[filled].id = id;
    data[filled].key = key;
    data[filled].pData = pv;

    mapping->insert(id, &data+filled);
    percolateUp(filled);
    return 0;
}

int heap::setKey(const std::string &id, int key){
    bool exists = false;
    node *np = (node *)(mapping->getPointer(id, &exists));

    if(!exists) return 1;

    int pos = getPos(np);
    data[pos].key = key;
    
    //Re-ordering may be necessary
    bool ord =  ((pos * 2 <= filled ) && (data[pos].key > data[pos*2].key)) ||
                ((pos*2 + 1 <= filled) && (data[pos].key > data[pos*2 + 1].key)) || 
                (pos == 1);
    if(ord) percolateDown(pos);
    else if(data[pos].key < data[pos/2].key) percolateUp(pos);
    return 0;
}

int heap::deleteMin(std::string *pId, int *pKey, void *ppData){
    if(filled == 0) return 1;

    if(pId) *pId = data[1].id;
    if(pKey) *pKey = data[1].key;
    if(ppData) *(static_cast<void **> (ppData)) = data[1].pData;

    mapping->remove(data[1].id);

    data[1] = data[filled];
    filled--;

    //Re-ordering WILL be necessary
    percolateDown(1);
    return 0;
}

int heap::remove(const std::string &id, int *pKey, void *ppData){
    bool exists = false;
    node *np = (node *)(mapping->getPointer(id, &exists));

    if(!exists) return 1;
    int pos = getPos(np);

    if(pKey) *pKey = data[pos].key;
    if(ppData) *(static_cast<void **>(ppData)) = data[pos].pData;

    mapping->remove(id);

    data[pos] = data[filled];
    mapping->setPointer(data[pos].id, &data[pos]);

    filled--;

    //Re-ordering may be necessary
    bool ord =  ((pos * 2 <= filled ) && (data[pos].key > data[pos*2].key)) ||
                ((pos*2 + 1 <= filled) && (data[pos].key > data[pos*2 + 1].key)) || 
                (pos == 1);
    if(ord) percolateDown(pos);
    else if(data[pos].key < data[pos/2].key) percolateUp(pos);
    return 0;
}

void heap::percolateUp(int posCur){
    int hole = posCur;
    node tmp;

    for(tmp = data[hole]; hole > 1 && tmp.key < data[hole/2].key; hole /= 2){
        data[hole] = data[hole/2];
        mapping->setPointer(data[hole].id, &data[hole]);
    }
    
    data[hole] = tmp; //Save the second half of the swap for later (I mean after the end of above loop)
    mapping->setPointer(data[hole].id, &data[hole]);
}

void heap::percolateDown(int posCur){
    int hole = posCur;
    node tmp;

    int child = hole * 2;

    for(tmp = data[hole]; hole * 2 <= filled; hole = child){
        child = hole * 2;
        if(child != filled && data[child+1].key < data[child].key)
            child++;

        if(data[child].key < tmp.key){
            data[hole] = data[child];
            mapping->setPointer(data[hole].id, &data[hole]);
        }else break;
    }
    data[hole] = tmp; //Second half of swap
    mapping -> setPointer(data[hole].id, &data[hole]);
}

int heap::getPos(node *pn){
    int pos = pn - &data[0];
    return pos;
}
