#include "hashmap.h"
#include "dynamic_array/array.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    array *arr;
} bucket;

struct hashmap {
    size_t elem_size;
    size_t len;
    bucket buckets[BUCKET_COUNT];
};

// FNV-1a 64-bit
// Reference: https://github.com/lcn2/fnv
static uint64_t fnv1a(const char *str) {
    uint64_t hash = UINT64_C(14695981039346656037);

    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= UINT64_C(1099511628211);
    }

    return hash;
}

static size_t bucket_index(const char *key) {
    return fnv1a(key) % BUCKET_COUNT;
}

hashmap *hash_create(size_t data_size) {
    if (data_size == 0)
        return NULL;

    hashmap *h = calloc(1, sizeof *h);
    if (h == NULL)
        return NULL;

    h->elem_size = data_size;

    for (size_t i = 0; i < BUCKET_COUNT; i++) {
        h->buckets[i].arr = array_create(sizeof(hash_data));

        if (h->buckets[i].arr == NULL) {
            for (size_t j = 0; j < i; j++)
                array_destroy(h->buckets[j].arr);

            free(h);
            return NULL;
        }
    }
    return h;
}

int hash_put(hashmap *h, hash_data data) {
    if (h == NULL || data.key == NULL || data.value == NULL)
        return -1;

    size_t index = bucket_index(data.key);
    array *arr = h->buckets[index].arr;

    for (size_t i = 0; i < arr->len; i++) {
        hash_data entry;

        if (array_get(arr, i, &entry) != 0)
            return -1;

        if (strcmp(entry.key, data.key) == 0) {
            memcpy(entry.value, data.value, h->elem_size);
            return 0;
        }
    }

    size_t key_len = strlen(data.key);

    if (key_len == SIZE_MAX)
        return -1;

    char *key_copy = malloc(key_len + 1);
    if (key_copy == NULL)
        return -1;

    memcpy(key_copy, data.key, key_len + 1);

    void *value_copy = malloc(h->elem_size);
    if (value_copy == NULL) {
        free(key_copy);
        return -1;
    }

    memcpy(value_copy, data.value, h->elem_size);

    hash_data entry = {.key = key_copy, .value = value_copy};

    if (array_push(arr, &entry) != 0) {
        free(key_copy);
        free(value_copy);
        return -1;
    }

    h->len++;
    return 0;
}

int hash_get(const hashmap *h, const char *key, void *out) {
    if (h == NULL || key == NULL || out == NULL)
        return -1;

    size_t index = bucket_index(key);
    array *arr = h->buckets[index].arr;

    for (size_t i = 0; i < arr->len; i++) {
        hash_data entry;

        if (array_get(arr, i, &entry) != 0)
            return -1;

        if (strcmp(entry.key, key) == 0) {
            memcpy(out, entry.value, h->elem_size);
            return 0;
        }
    }

    return -1;
}

void hash_destroy(hashmap *h) {
    if (h == NULL)
        return;

    for (size_t i = 0; i < BUCKET_COUNT; i++) {
        array *arr = h->buckets[i].arr;

        if (arr == NULL)
            continue;

        for (size_t j = 0; j < arr->len; j++) {
            hash_data entry;

            if (array_get(arr, j, &entry) == 0) {
                free(entry.key);
                free(entry.value);
            }
        }

        array_destroy(arr);
    }

    free(h);
}
