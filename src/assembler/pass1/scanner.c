/*
THIS IS PASS 1 OF THE TWO PASS APPROACH

We read through each line, tracking the current memory address (making sure to ignore comments).
When we enccounter a label, we add it, and the current address, to the symbol table.
*/


// Declaration needed to avoid compiler warning
void scanner_placeholder(void) {
    return;
}
