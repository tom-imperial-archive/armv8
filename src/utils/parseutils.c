#include "parseutils.h"
#include <ctype.h>

char *trim_leading_whitespace(char *s) {
    while (isspace(*s)) s++;
    return s;
}
