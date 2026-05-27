#ifndef HASHSET_H
#define HASHSET_H
#include "common/util.h"

typedef struct {
    uint8 **entries;
    int capacity;
    int size;
} AddressSet;

AddressSet* create_set();
void insert_address(AddressSet *set, uint8 *address);
void free_set(AddressSet *set);
uint8** get_all_addresses(AddressSet *set);

#endif
