#ifndef HASHSET_H
#define HASHSET_H
#include "utils/types.h"

typedef struct {
    uint8 **entries;
    int capacity;
    int size;
} AddressSet;

extern AddressSet *create_set();
extern void insert_address(AddressSet *set, uint8 *address);
extern void free_set(AddressSet *set);
extern uint8 **get_all_addresses(AddressSet *set);

#endif
