/*hopscotch hash: algorithm created by Maurice Herlihy, Nir Shavit and 
/Moran Tzafrir in 2008. All algorithmic ideas are attributed to them. I merely
implemented their brilliant scheme. 

For more info on the algorithm,
vist https://en.wikipedia.org/wiki/Hopscotch_hashing*/

#include "hashmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N_SIZE 32
#define MAX_SEARCH 250

//nice property of hopscotch hash map: if an elem is inserted successfully, its 
//index is always greater than its hashed_index AND within NSIZE-1 of the 
//hashed_index, where NSIZE is the neighborhood size (like the "bucket" of
//an index). 
struct elem_header {
  char *key;
  void *val;
  size_t hashed_index;
};
typedef struct elem_header* elem;

//use a single array as the hash map.
struct hmap_header {
  size_t size;
  size_t capacity;
  elem *array;
  void** deleted;
};

//sdbm hash
//source: http://www.cse.yorku.ca/~oz/hash.html
size_t hash(hmap H, char *key){
  size_t hash = 0;
  int c;
  
  char* start = key;
  while ((c = *start)){
    hash = c + (hash << 6) + (hash << 16) - hash;
    start++;
  }

  return hash % H->capacity;
}

hmap init(size_t capacity){
  hmap H = malloc(sizeof(struct hmap_header));
  elem *array = calloc(capacity, sizeof(elem));
  H->size = 0;
  H->capacity = capacity;
  H->array = array;
  H->deleted = malloc(sizeof(void*));
  return H;
}

bool set(hmap H, char *key, void *val){
  //hash map full
  if(H->size == H->capacity)
    return false;

  elem e = malloc(sizeof(struct elem_header));
  e->key = key;
  e->val = val;
  size_t index = hash(H, key);
  e->hashed_index = index;
    
  elem *array = H->array;

  //if the elem hashed to an empty slot, insert is successful!
  if(!array[index]){
    array[index] = e;
    (H->size)++;
    return true;
  }

  //Otherwise, linearly probe for the next empty entry, but within the 
  //search bound, which is limited by MAX_SEARCH
  size_t limit = index + MAX_SEARCH; 
  size_t capacity = H->capacity;
  size_t hashed_index = index;
  while(index < limit && index < capacity){
    if(!array[index]) //empty slot found
      break;
    else{
      elem e = array[index];
      //if the key was already inserted, just update it
      if(strcmp(e->key,key) == 0){
        e->val = val;
        return true;
      }
    }
    index++;
  }
  
  //exceeded search limit or capacity
  if(index == capacity || index == limit){
    free(e);
    return false;
  }
  
  //if the next empty slot is within the neighborhood of the hashed_index,
  //just insert it there
  if(index - hashed_index < N_SIZE){
    array[index] = e;
    (H->size)++;
    return true;
  }

  size_t curr_index, poss_index;
  size_t nh = N_SIZE - 1;
  bool success = false;

  array[index] = 0;
  size_t temp = index;

  //while index is out of bounds (of the neighborhood), keep trying to "swap" 
  //position with elements whose index is closer to the hashed_index, but 
  //keeping in mind that the swap has to preserve the property of the hash map:
  //any inserted element is always found at its hashed_index or in the next 
  //NSIZE-1 indices.
  while(index - hashed_index > nh && index < capacity){
    poss_index = index - nh;
    curr_index = poss_index;
    while(curr_index < index){
      elem curr_elem = array[curr_index];
      //can swap the curr_elem
      if(curr_elem->hashed_index >= poss_index){
        array[index] = array[curr_index];
        array[curr_index] = e;
        //curr_index satisfies hash map property
        if(curr_index - hashed_index < N_SIZE){
          success = true;
          (H->size)++;
        }
        break;
      }
      curr_index++;
    }
    //didn't find any possible swaps, abort search
    if(curr_index == index)
      break;
    index = curr_index;
  }
  
  //didn't insert
  if(temp == index)
    array[temp] = 0;
  
  //made some swaps, but ultimately had to abort insert since the best index
  //was still too far away from hashed_index
  if(!success && (curr_index != temp))
    array[curr_index] = 0;

  if(!success)
    free(e);

  return success;
}

void* get(hmap H, char *key){
  elem *array = H->array;
  size_t index = hash(H, key);
  size_t nh = index + N_SIZE;
  size_t capacity = H->capacity;
  //very convenient get: any inserted element is always found at its 
  //hashed_index or in the next NSIZE-1 indices. Makes get(H,key) O(1).
  while(index < nh && index < capacity){
    elem e = array[index];
    if(e != NULL && strcmp(e->key, key) == 0){
      return e->val;
    }
    index++;
  }
  return NULL;
}

void* delete(hmap H, char *key){
  elem *array = H->array;
  size_t index = hash(H, key);
  size_t nh = index + N_SIZE - 1;
  size_t capacity = H->capacity;
  while(index < nh && index < capacity){
    elem e = array[index];
    if(e != NULL && strcmp(e->key, key) == 0){
      array[index] = 0;
      H->size = (H->size)--;
      memcpy(H->deleted, &(e->val), 8);
      free(e);
      return *(H->deleted);
    }
    index++;
  }
  return NULL;
}

float load(hmap H){
  size_t size = H->size;
  double capacity = (double) H->capacity;
  return size/capacity;
}

void free_map(hmap H){
  size_t capacity = H->capacity;
  elem *array = H->array;
  for(size_t i = 0; i < capacity; i++){
    if(array[i])
      free(array[i]);
  }
  free(H->deleted);
  free(H->array);
  free(H);
}


