/*
Here, we take an instruction, represented by a struct, and turn it into the actual 32-bit representation.
This will ultimately be achieved by bitwise operations and masks.

Essentially, we will have one big switch, like we did in execute,
    but the work being done is like the opposite of what was done in decode.
*/

// Declaration needed to avoid compiler warning
void encode_placeholder(void) {
    return;
}
