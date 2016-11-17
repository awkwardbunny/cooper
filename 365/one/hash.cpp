#include "hash.h"
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;

hashTable::hashTable(int size){
  capacity = getPrime(size * 2);
  data.resize(capacity);
  filled = 0;
}

int hashTable::insert(const std::string &key, void *pv){
  if(contains(key)) return 1;

  int position = hash(key) % capacity;

  while(data[position].isOccupied){             //this loop will find the next unoccupied
    if(data[position].key.compare(key) == 0) {  //unless it finds a deleted position with the same key before it finds an unoccupied
      data[position].isDeleted = false;
      data[position].pv = pv;
      return 0;
    }
      
    position = (position + 1) % capacity;   //loop over if necessary
  }

  data[position].isOccupied = true;
  data[position].isDeleted = false;
  data[position].key = key;
  data[position].pv = pv;
  filled++;
  
  if (((float)filled/(float)capacity) > 0.5){ //rehash is necessary (if more than 50% filled)
    return rehash() ? 0 : 2;
  }

  return 0;
}

bool hashTable::contains(const std::string &key){
  return (findPos(key) != -1);
}

//http://www.cse.yorku.ca/~oz/hash.html -- DJB2XOR hash
unsigned int hashTable::hash(const string &key){
  unsigned int hash = 5381;

  const char *str = key.c_str();
  while(*str){
    hash = ((hash << 5) + hash) ^ *str++; //multiplying hash by 33 by shift left 5 and add itself
  }

  return hash;
}

int hashTable::findPos(const string &key){
  int position = hash(key) % capacity;

  if(!data[position].key.compare(key)){
    return position;
  }

  while(position < capacity && data[position].isOccupied && data[position].key.compare(key))  //this loop tries to find an occupied slot with same key
    position++;
  
  if(position == capacity){             //if the end of the table is reached,
    position = 0;                       //restart the search from the beginning
    while(position < capacity && data[position].isOccupied && data[position].key.compare(key))
      position++;
  }

  return (!data[position].key.compare(key) && !data[position].isDeleted) ? position : -1; //if the keys still don't match, entry doesn't exist and return -1
}

bool hashTable::rehash(){
  vector<hashItem> oldTable = data;
  
  for(int i = 0; i < capacity; i++){
    data[i].isOccupied = false;
    data[i].isDeleted = true;
  }

  capacity = getPrime(capacity * 2);
  data.resize(capacity);
  if(capacity != data.size()) return false;
  filled = 0;

  for (int i = 0; i < oldTable.size(); i++){
    if (oldTable[i].isOccupied && !oldTable[i].isDeleted){   //only re-insert valid entries
      insert(oldTable[i].key, oldTable[i].pv);
    }
  }
  return true;
} 

unsigned int hashTable::getPrime(int size){
  //http://planetmath.org/goodhashtableprimes
  vector<unsigned int> primes = {
    53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 
    196613, 393241, 786433, 1572869, 3145739, 786433, 1572869, 3145739, 6291469, 12582917,  25165843, 
    50331653, 100663319, 201326611, 402653189, 805306457, 1610612741
  };

  if(size >= primes.back()) return primes.back();  //if size is larger than or equal to the largest prime available, return the largest

  int i = -1;
  while(primes[++i] < size);     //loop until prime is larger than size

  return primes[i];
}
