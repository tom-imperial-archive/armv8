#include <stdlib.h>
#include "hashset.h"
#include "util.h"

#define BIG_PRIME 26544357617u
#define INITIAL_CAPACITY 1024


AddressSet* create_set(void) {
    AddressSet *set = malloc(sizeof(AddressSet));

    set->capacity = INITIAL_CAPACITY;
    set->size = 0;

    set->entries = calloc(set->capacity, sizeof(char *));

    return set;
}

static int get_hash_index(char *address, int current_capacity) {
    // Cast to an integer, so we can use %
    uintptr numeric_address = (uintptr)address;
    // Multiply by a prime to ensure more even hashes
    return (numeric_address * BIG_PRIME) % current_capacity;
}

static void resize_set(AddressSet *set) {
    int old_capacity = set->capacity;
    char **old_entries = set->entries;

    // Resizing doubles capacity
    set->capacity = old_capacity * 2;

    set->entries = calloc(set->capacity, sizeof(char *));
    set->size = 0;

    for (int i = 0; i < old_capacity; i++) {
        if (old_entries[i] != NULL) {
            insert_address(set, old_entries[i]);
        }
    }

    free(old_entries);
}

void insert_address(AddressSet *set, char *address) {
    if (address == NULL) return;

    // Check if resize is required
    if (set->size >= (set->capacity * 3) / 4) {
        resize_set(set);
    }

    int index = get_hash_index(address, set->capacity);

    // Find an empty slot
    while (set->entries[index] != NULL) {
        if (set->entries[index] == address) {
            return;
        }

        // Wrap around if necessary
        index = (index + 1) % set->capacity;
    }

    set->entries[index] = address;
    set->size++;
}

void free_set(AddressSet *set) {
    if (set == NULL) return;

    if (set->entries != NULL) {
        free(set->entries);
    }

    free(set);
}

char** get_all_addresses(AddressSet *set) {
    if (set == NULL || set-> size == 0) {
        return NULL;
    }

    char **results = malloc(set->size * sizeof(char *));

    int result_index = 0;
    for (int i = 0; i < set->capacity; i++) {
        if (set->entries[i] != NULL) {
            results[result_index] = set->entries[i];
            result_index++;
        }
    }
    return results;
}
