/*
THIS IS PASS 2 OF THE TWO PASS APPROACH

Here, we tokenise each line in the file, and build it into a struct
We are reusing the same structs as we did in deocde (now stored in common/instruction.h)
Note operands.c provides helpers that we should use here.

We should use function pointers here (as suggested in the spec) to avoid a very messy if/then/else structure.
*/

// Declaration needed to avoid compiler warning
void parser_placeholder(void) {
    return;
}
