// MODULES / UTILS / STRING.C

#include "string.h" // modules/utils/string.h

#include <stdlib.h>
#include <stdio.h>


char *_utils_string_vaprintf_to_string(const char *format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    
    int len = vsnprintf(NULL, (size_t)0, format, args_copy);
    va_end(args_copy);
    
    if (len < 0) return NULL;
    
    char *str = malloc((size_t)len + 1);
    if (!str) return NULL;
    
    vsnprintf(str, (size_t)len + 1, format, args);
    
    return str;
}
