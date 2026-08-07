// MODULES / UTILS / ARRAY.C

#include <stdio.h>
#include <string.h>


char *_utils_array_to_stringbuffer(char **array_params, int count, char *buffer_params, size_t buffer_size, int array_params_start, int array_params_end) {
  if (array_params == NULL || buffer_params == NULL || buffer_size == 0) { return NULL; }
  if (array_params_end == 0) { array_params_end = count-1; }
  int offset = 0;

  offset = snprintf(buffer_params, buffer_size, "[");
  for (int i = array_params_start; i <= array_params_end; i++) {
    if (array_params[i] == NULL) continue;

    int written = snprintf(buffer_params + offset, buffer_size - offset, "%s\"%s\"", (i > array_params_start ? "," : ""), array_params[i]);
    
    if (written < 0 || (size_t)written >= buffer_size - offset) { break; }
    offset += written;
  }

  if (offset < buffer_size - 1) { buffer_params[offset++] = ']'; buffer_params[offset] = '\0'; }

  return buffer_params;
}

