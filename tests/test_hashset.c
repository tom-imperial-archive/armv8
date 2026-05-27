#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "common/hashset.h"

void test_creation() {
    AddressSet *set = create_set(1024);

    assert(set != NULL);
    assert(set->capacity == 1024);
    assert(set->size == 0);

    free_set(set);
    printf("Test creation: OK\n");
}

void test_insertion() {
    AddressSet *set = create_set(10);

    // Dummy data
    uint8 var_a = 'A';
    uint8 var_b = 'B';
    insert_address(set, &var_a);
    assert(set->size == 1);

    insert_address(set, &var_b);
    assert(set->size == 2);

    free_set(set);

    printf("Test insertion: OK\n");
}

void test_get_all_addresses() {
    AddressSet *set = create_set(10);

    // Dummy data
    uint8 var_a = 'A';
    uint8 var_b = 'B';
    uint8 var_c = 'C';

    insert_address(set, &var_a);
    insert_address(set, &var_b);
    insert_address(set, &var_c);

    uint8 **results = get_all_addresses(set);

    assert(results != NULL);

    // We cannot be certain about the order the addresses are stored
    int found_a = 0, found_b = 0, found_c = 0;

    for (int i = 0; i < set->size; i++) {
        if (results[i] == &var_a) found_a = 1;
        if (results[i] == &var_b) found_b = 1;
        if (results[i] == &var_c) found_c = 1;
    }

    assert(found_a == 1);
    assert(found_b == 1);
    assert(found_c == 1);

    free(results);
    free_set(set);

    printf("Test get all addresses: OK\n");
}

int test_hashset(void) {
    printf("Hashset Tests\n");
    printf("-------------\n");
    test_creation();
    test_insertion();
    test_get_all_addresses();
    printf("-------------\n");
    printf("All tests passed\n");
    return 0;
}
