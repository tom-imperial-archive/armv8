#include "test_execute.h"
#include <stdio.h>
#include "utils/types.h"
#include "emulator/execute/execute.h"
#include "test_dpii.h"
#include "test_dpir.h"
#include "test_branch.h"
#include "test_load_literal.h"
#include "test_single_data_transfer.h"


int main(void)
{
    printf("Execute Tests\n");
    printf("-------------\n");

    test_dpii();
    test_dpir();
    test_branch();

    test_load_literal();
    test_single_data_transfer();
    printf("-------------\n");
    printf("All tests passed\n");
    return 0;
}
