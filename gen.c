#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define KEY_LENGTH 33
#define VAL_LENGTH 201
void rand_string(size_t length, char* key){
  for(size_t i = 0; i < length; i++){
    key[i] = (char)(rand()%95 + 32);
  }
  key[length] = '\0';
}

int main(){
  FILE *fp = fopen("kv_set_1.txt", "a");
  char key[KEY_LENGTH];
  //used for when val is a string
  char val[VAL_LENGTH];

  if(fp == NULL){
    printf("couldn't open or create file");
    exit(1);
  }
  srand(time(NULL));
  for(size_t i = 0; i < 5000; i++){
    rand_string(32, key);
    rand_string(200, val);
    fprintf(fp,"%s %d\n", key, rand());
  }

  fclose(fp);
  return 0;
}
