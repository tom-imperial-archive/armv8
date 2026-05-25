#include <stdio.h>
#include <stdlib.h>
#include "common/util.h"
#include "emulator/decode/readfile.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Require file in: <file_in> [file_out]\n");
        return 1;
    }

    size_t count;
    uint32 *memory = readfile(argv[1], &count);
    if (!memory) return 1;


    return EXIT_SUCCESS;
}
