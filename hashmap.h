#include <stdbool.h>
#include <stdlib.h>

typedef struct hmap_header* hmap;

hmap init(size_t capacity);
bool set(hmap H, char *key, void *val);
void* get(hmap H, char *key);
void* delete(hmap H, char *key);
float load(hmap H);
void free_map(hmap H);


