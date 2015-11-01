#include "hashmap.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define KEY_LENGTH 33
#define VAL_SIZE 201
#define BUF_LENGTH 235

#define SET_SIZE_1 5000
#define SET_SIZE_2 5000
typedef bool val_eq(void* v1, void* v2);

bool test_get(hmap H, char (*keys)[KEY_LENGTH], void** vals, size_t n, val_eq eq,size_t*hit,size_t* miss){
  bool status[n];
  for(size_t i = 0; i < n; i++){
    status[i] = set(H, keys[i], vals[i]);
    if(status[i]) (*hit)++;
    else (*miss)++;
  }
  for(size_t i = 0; i < n; i++){
    if(status[i] && !eq(get(H,keys[i]), vals[i])){
      return false;
    }
  }

  return true;
}

bool test_delete(hmap H, char (*keys)[KEY_LENGTH], void** vals, size_t n, val_eq eq){
  void* val;
  for(size_t i = 0; i < n; i++){
    if((val = delete(H,keys[i]))){
      if(!eq(val, vals[i]))
        return false;
    }
  }
  return true;
}

bool string_eq(void* v1, void* v2){
  if(v1 == NULL || v2 == NULL) return false;
  return strcmp((char*)v1, (char*)v2) == 0;
}

bool long_eq(void* v1, void* v2){
  if(v1 == NULL || v2 == NULL) return false;
  return (long)v1 == (long)v2;
}

int main(){
  char (*keys)[KEY_LENGTH] = malloc(KEY_LENGTH*SET_SIZE_2);
  char buf[BUF_LENGTH];
  void** vals = malloc(VAL_SIZE*SET_SIZE_2);
  
  size_t hit=0, miss=0;

  //test set 1, val as longs, 5000 pairs
  hmap H_1 = init(SET_SIZE_1);
  FILE* fp_1 = fopen("kv_set_1.txt", "r");
  if(fp_1 == NULL){
    printf("couldn't open file");
    exit(1);
  }

  size_t i = 0;
  while(fgets(buf,BUF_LENGTH,fp_1)){
    vals[i] = (void*)(long)atoi(buf+33);
    strncpy(keys[i], buf, 32);
    keys[i][32] = '\0'; 
    i++;
  }

  assert(test_get(H_1, keys, vals, SET_SIZE_1, long_eq, &hit, &miss));
  printf("Insertion--- successes: %zu  fails: %zu  ratio: %f\n", hit, miss, (float)hit/(hit+miss));
  assert(test_delete(H_1, keys, vals, SET_SIZE_1, long_eq));
  free_map(H_1);

  //test set 2, val as strings, 5000 pairs
  hmap H_2 = init(SET_SIZE_2);

  FILE* fp_2 = fopen("kv_set_2.txt", "r");
  if(fp_2 ==NULL){
    printf("couldn't open file");
    exit(1);
  }

  i = 0;
  while(fgets(buf,BUF_LENGTH,fp_2)){
    vals[i] = (void*)(buf+33);
    strncpy(keys[i], buf, 32);
    keys[i][32] = '\0';
    i++;
  }

  hit = 0; miss = 0;
  assert(test_get(H_2, keys, vals, SET_SIZE_2, string_eq, &hit, &miss));
  printf("Insertion--- successes: %zu  fails: %zu  ratio: %f\n", hit, miss, (float)hit/(hit+miss));
  assert(test_delete(H_2, keys, vals, SET_SIZE_2, string_eq));
  free_map(H_2);

  fclose(fp_1);
  fclose(fp_2);
  free(keys);
  free(vals);
  return 0;
}
