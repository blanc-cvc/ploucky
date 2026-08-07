// MODULES / UTILS / STRING.C

#include "string.h" // modules/utils/string.h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


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

char *_utils_string_remove_start(char *str, int n) {
  int len = strlen(str);
  if (n >= len) {
    str[0] = '\0';
    return str;
  }
  memmove(str, str + n, len - n + 1);
  
  return str;
}

char *_utils_string_remove_end(char *str, int n) {
  int len = strlen(str);
  if (n >= len) {
    str[0] = '\0';
    return str;
  }
  str[len - n] = '\0';
  
  return str;
}

char *_utils_string_remove_at(char *str, int pos, int n) {
  int len = strlen(str);
  if (pos >= len || pos < 0) return str;
  
  if (pos + n > len) n = len - pos;

  memmove(str + pos, str + pos + n, len - (pos + n) + 1);
  
  return str;
}

int _utils_string_has_char(char *str, const char c) {
  int len = strlen(str);
  int count = 0;
  for (int i = 0; i < len; i++) {
    if (str[i] == c) { count++; }
  }
  return count;
}

char *_utils_string_trim(char *str) {
  if (str == NULL) return NULL;

  char *start = str;
  while (*start && isspace((unsigned char)*start)) {
    start++;
  }

  if (*start == '\0') {
    str[0] = '\0';
    return str;
  }

  char *end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end)) {
    end--;
  }

  *(end + 1) = '\0';

  if (start != str) {
    memmove(str, start, (end - start + 2));
  }

  return str;
}


