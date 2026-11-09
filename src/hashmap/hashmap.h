#ifndef CHOPLIB_HASH_MAP
#define CHOPLIB_HASH_MAP

#include <stddef.h>

#ifndef BUCKET_COUNT
#define BUCKET_COUNT 16
#endif

#if BUCKET_COUNT < 1
#error "BUCKET_COUNT must be positive"
#endif

typedef struct hashmap hashmap;

typedef struct hash_data {
    char *key;
    void *value;
} hash_data;

hashmap *hash_create(size_t data_size);

int hash_put(hashmap *h, hash_data data);

int hash_get(const hashmap *h, const char *key, void *out);

void hash_destroy(hashmap *h);

#endif

