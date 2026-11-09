# hash-map

A small, generic hash map implementation in C, built to explore hashing, collision resolution, memory management, and different hash table architectures.

Part of [choplib](https://github.com/choplib), a collection of experimental C libraries and systems programming projects.

**Status: Functional prototype — under development.**

## Overview

The current implementation uses **separate chaining** with dynamically allocated buckets.

Each bucket stores key-value entries using [choplib/dynamic-array](https://github.com/choplib/dynamic-array), included as a Git submodule.

The hash map supports string keys and values of arbitrary fixed size. Keys and values are copied into independently owned memory, allowing the original data to be modified or released after insertion.

### Current architecture

```text
                  hashmap
                     |
                FNV-1a hash
                     |
              hash % bucket_count
                     |
                     v
       +-----------------------------+
       | Bucket 0 -> dynamic-array   |
       | Bucket 1 -> dynamic-array   |
       | Bucket 2 -> dynamic-array   |
       | Bucket 3 -> dynamic-array   |
       |            ...              |
       | Bucket N -> dynamic-array   |
       +-----------------------------+
                     |
                     v
          [(key, value), ...]
```

Each bucket can grow independently through the dynamic array implementation.

Collisions are resolved by storing multiple entries in the same bucket and comparing their keys during lookup.

## Features

- Generic fixed-size value storage through `void *`.
- Null-terminated string keys.
- 64-bit FNV-1a hashing.
- Separate chaining for collision resolution.
- Dynamic arrays for bucket storage.
- Independent copies of keys and values.
- Updating existing values when inserting duplicate keys.
- Automatic cleanup of owned allocations.
- Configurable initial bucket count at compile time.

The current implementation does not support removal, dynamic bucket resizing, or open addressing.

## API

```c
typedef struct hashmap hashmap;

typedef struct hash_data {
    char *key;
    void *value;
} hash_data;

hashmap *hash_create(size_t data_size);

int hash_put(hashmap *h, hash_data data);

int hash_get(const hashmap *h, const char *key, void *out);

void hash_destroy(hashmap *h);
```

## Example

```c
#include "hashmap.h"
#include <stdio.h>

int main(void) {
    hashmap *map = hash_create(sizeof(int));
    if (!map)
        return 1;

    int value = 42;

    if (hash_put(map, (hash_data){
        .key = "answer",
        .value = &value
    }) != 0) {
        hash_destroy(map);
        return 1;
    }

    int result = 0;

    if (hash_get(map, "answer", &result) == 0)
        printf("answer = %d\n", result);

    hash_destroy(map);
    return 0;
}
```

## Dependencies

The project uses [choplib/dynamic-array](https://github.com/choplib/dynamic-array) as a Git submodule.

Clone the repository with its dependencies:

```sh
git clone --recurse-submodules https://github.com/choplib/hash-map.git
cd hash-map
```

If the repository has already been cloned:

```sh
git submodule update --init --recursive
```

## Build

The development Makefile uses Clang with AddressSanitizer and UndefinedBehaviorSanitizer enabled.

```sh
make
make run
```

To remove generated binaries:

```sh
make clean
```

The current build is intended for development and experimentation rather than installation as a system library.

## Testing

The initial implementation has passed the following functional tests:

| Test | Result |
|---|---|
| Basic insertion and lookup | Passed |
| Updating an existing key | Passed |
| Missing key lookup | Passed |
| 10,000 insertions and lookups | Passed |
| Generic struct storage | Passed |
| Independent key and value copies | Passed |

These tests exercise the core API, including collisions that arise when inserting many entries into a fixed number of buckets.

The development build supports sanitizers, but successful functional tests do not establish complete memory safety.

Dedicated allocation-failure, collision stress, and automated regression test coverage remain future work.

## Design considerations

### Hash function

The current implementation uses 64-bit FNV-1a, a non-cryptographic hash function.

Reference implementation: [lcn2/fnv](https://github.com/lcn2/fnv).

The resulting hash is mapped to a bucket index using:

```c
index = fnv1a(key) % BUCKET_COUNT;
```

FNV-1a was chosen for its simplicity and suitability for an initial experimental implementation.

It is not designed to resist maliciously constructed collision attacks.

### Memory ownership

The hash map owns copies of both keys and values.

Each stored entry contains pointers to separately allocated memory:

```c
typedef struct {
    char *key;
    void *value;
} hash_data;
```

The dynamic array stores copies of these entry structures.

`hash_destroy()` releases the owned allocations and destroys the underlying bucket arrays.

### Current limitations

Bucket count is currently fixed at compile time.

Although the dynamic arrays can grow, increasing the number of stored entries also increases the average number of entries per bucket.

Lookups therefore become more expensive as the map grows.

Dynamic bucket resizing and rehashing are planned to address this limitation.

## Roadmap

### Phase 1 — Basic implementation

- [x] Implement 64-bit FNV-1a hashing.
- [x] Implement separate chaining.
- [x] Integrate `choplib/dynamic-array`.
- [x] Support generic values.
- [x] Implement `hash_create()`.
- [x] Implement `hash_put()`.
- [x] Implement `hash_get()`.
- [x] Implement `hash_destroy()`.
- [x] Support updates to existing keys.
- [x] Verify basic insertion, lookup, and data integrity.

### Phase 2 — Dynamic resizing

- [ ] Replace fixed bucket storage with dynamically allocated storage.
- [ ] Track the number of stored entries.
- [ ] Calculate the load factor.
- [ ] Introduce a configurable resizing threshold.
- [ ] Implement bucket expansion.
- [ ] Redistribute existing entries through rehashing.
- [ ] Test data integrity before and after resizing.

### Phase 3 — Alternative collision resolution

- [ ] Implement open addressing.
- [ ] Introduce linear probing.
- [ ] Handle collisions within the slot table.
- [ ] Implement table expansion for open addressing.
- [ ] Support compile-time strategy selection.
- [ ] Maintain a consistent public API across implementations.

Planned configuration:

```c
#define HASHMAP_CHAINING 1
#define HASHMAP_OPEN_ADDRESSING 2

#ifndef HASHMAP_STRATEGY
#define HASHMAP_STRATEGY HASHMAP_CHAINING
#endif
```

### Phase 4 — Additional functionality and testing

- [ ] Evaluate key removal for both strategies.
- [ ] Test deliberately colliding keys.
- [ ] Test invalid arguments and allocation failures.
- [ ] Add dedicated automated unit tests.
- [ ] Add sanitizer-backed stress tests.
- [ ] Verify memory cleanup under failure conditions.

### Phase 5 — Benchmarking

- [ ] Measure insertion performance.
- [ ] Measure lookup performance for existing and missing keys.
- [ ] Compare separate chaining and open addressing.
- [ ] Measure memory consumption.
- [ ] Investigate the impact of load factor.
- [ ] Compare behavior across different element counts and value sizes.
- [ ] Document the results and their limitations.

## Scope

This project is intended for learning and experimentation, not production use.

The implementation is currently single-threaded, uses a non-cryptographic hash function, and does not yet provide comprehensive error-path validation or performance guarantees.

The roadmap describes possible future development rather than a fixed release schedule.

---

Developed by [M1ralai](https://github.com/M1ralai) as part of [choplib](https://github.com/choplib).
