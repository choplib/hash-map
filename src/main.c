#include "hashmap/hashmap.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 10000

typedef struct {
    int id;
    double score;
    char name[32];
} student;

static void test_basic(void) {
    hashmap *h = hash_create(sizeof(int));
    assert(h != NULL);

    int age = 21;
    int score = 95;
    int result = 0;

    assert(hash_put(h, (hash_data){"age", &age}) == 0);
    assert(hash_put(h, (hash_data){"score", &score}) == 0);

    assert(hash_get(h, "age", &result) == 0);
    assert(result == 21);

    assert(hash_get(h, "score", &result) == 0);
    assert(result == 95);

    puts("[PASS] Basic insertion and lookup");

    hash_destroy(h);
}

static void test_update(void) {
    hashmap *h = hash_create(sizeof(int));
    assert(h != NULL);

    int first = 42;
    int second = 100;
    int result = 0;

    assert(hash_put(h, (hash_data){"key", &first}) == 0);
    assert(hash_put(h, (hash_data){"key", &second}) == 0);

    assert(hash_get(h, "key", &result) == 0);
    assert(result == 100);

    puts("[PASS] Updating an existing key");

    hash_destroy(h);
}

static void test_missing(void) {
    hashmap *h = hash_create(sizeof(int));
    assert(h != NULL);

    int result = 0;

    assert(hash_get(h, "does-not-exist", &result) == -1);

    puts("[PASS] Missing key");

    hash_destroy(h);
}

static void test_many_elements(void) {
    hashmap *h = hash_create(sizeof(int));
    assert(h != NULL);

    char key[64];

    for (int i = 0; i < TEST_COUNT; i++) {
        int value = i * 3;

        snprintf(key, sizeof key, "item-%d", i);

        assert(hash_put(h, (hash_data){key, &value}) == 0);
    }

    for (int i = 0; i < TEST_COUNT; i++) {
        int result = -1;

        snprintf(key, sizeof key, "item-%d", i);

        assert(hash_get(h, key, &result) == 0);
        assert(result == i * 3);
    }

    puts("[PASS] 10,000 insertions and lookups");

    hash_destroy(h);
}

static void test_struct(void) {
    hashmap *h = hash_create(sizeof(student));
    assert(h != NULL);

    student alice = {
        .id = 1,
        .score = 92.5,
        .name = "Alice"
    };

    student bob = {
        .id = 2,
        .score = 87.0,
        .name = "Bob"
    };

    assert(hash_put(h, (hash_data){"alice", &alice}) == 0);
    assert(hash_put(h, (hash_data){"bob", &bob}) == 0);

    student result = {0};

    assert(hash_get(h, "alice", &result) == 0);
    assert(result.id == 1);
    assert(result.score == 92.5);
    assert(strcmp(result.name, "Alice") == 0);

    assert(hash_get(h, "bob", &result) == 0);
    assert(result.id == 2);
    assert(strcmp(result.name, "Bob") == 0);

    puts("[PASS] Generic struct storage");

    hash_destroy(h);
}

static void test_independent_copies(void) {
    hashmap *h = hash_create(sizeof(int));
    assert(h != NULL);

    char key[] = "original";
    int value = 42;

    assert(hash_put(h, (hash_data){key, &value}) == 0);

    strcpy(key, "modified");
    value = 999;

    int result = 0;

    assert(hash_get(h, "original", &result) == 0);
    assert(result == 42);

    assert(hash_get(h, "modified", &result) == -1);

    puts("[PASS] Independent key/value copies");

    hash_destroy(h);
}

int main(void) {
    puts("=== choplib/hashmap tests ===\n");

    test_basic();
    test_update();
    test_missing();
    test_many_elements();
    test_struct();
    test_independent_copies();

    puts("\nAll tests passed!");
    return 0;
}
