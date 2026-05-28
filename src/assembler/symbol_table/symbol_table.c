/*
This abstract data structure will act as a dictionary from labels to addresses.
It will be populated by scanner.c, and then used by parser.c.
Since there will be a very low number of labels, it suffices to implement this
    using an array, similarly to [(Label, Address)] in Haskell.
*/

// Declaration needed to avoid compiler warning
void symbol_table_placeholder(void) {
    return;
}
