#ifndef HASHSET_H
#define HASHSET_H
typedef struct {
    char **entries;
    int capacity;
    int size;
} AddressSet;

AddressSet* create_set();
void insert_address(AddressSet *set, char *address);
void free_set(AddressSet *set);
char** get_all_addresses(AddressSet *set);

#endif
