#include <stdlib.h>
#include <stdio.h>
#include <emulator/state/state.h>

int main(int argc, char **argv)
{
    State *state = init_state();
    offset_pc(state, 8);
    print_all_registers(state);
    destroy_state(state);
    return EXIT_SUCCESS;
}
