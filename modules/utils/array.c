// MODULES / UTILS / ARRAY.C

#include <stdio.h>
#include <string.h>


char *_utils_array_to_stringbuffer(char **array, int count, char *buffer, size_t buffer_size, int array_start, int array_end, const char *delimiter) {
  if (array == NULL || buffer == NULL || buffer_size == 0) { return NULL; }
  if (array_end == 0) { array_end = count-1; }
  int offset = 0;

  if (delimiter == ",") { offset = snprintf(buffer, buffer_size, "["); }
  for (int i = array_start; i <= array_end; i++) {
    if (array[i] == NULL) continue;


    int written = snprintf(buffer + offset, buffer_size - offset, (delimiter == "," ? "%s\"%s\"" : "%s%s"), (i > array_start ? delimiter : ""), array[i]);
    
    if (written < 0 || (size_t)written >= buffer_size - offset) { break; }
    offset += written;
  }

  if (offset < buffer_size - 1) { if (delimiter == ",") { buffer[offset++] = ']'; } buffer[offset] = '\0'; }

  return buffer;
}

