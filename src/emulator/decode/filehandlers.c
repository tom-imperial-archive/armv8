#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common/util.h"

uint32 *readfile(const char *path, size_t *count) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("Error opening input file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if ((size % 4) != 0) {
        fprintf(stderr, "Error: file size not multiple of 4\n");
        fclose(f);
        return NULL;
    }

    *count = size/4;
    uint32 *instrs = malloc(*count * sizeof(uint32));
    if (!instrs) {
        perror("malloc");
        fclose(f);
        return NULL;
    }

    if (fread(instrs, sizeof(uint32), *count, f) != *count) {
        fprintf(stderr, "Error: could not read all words\n");
        free(instrs);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return instrs;

}

bool writefile(const char *path, char *registers_out, char *nonzero_out) {
    FILE *f = fopen(path, "w");

    if (f == NULL) {
        perror("Error opening output file");
        return false;
    }

    fprintf(f, "%s\n%s\n", registers_out, nonzero_out);
    fclose(f);

    return true;
}
