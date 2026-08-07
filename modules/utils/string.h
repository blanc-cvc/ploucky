// MODULES / UTILS / STRING.H

#ifndef UTILS_STRING_H
  #define UTILS_STRING_H
  
  #include <stdarg.h>
  
  char *_utils_string_vaprintf_to_string(const char *format, va_list args);
  char *_utils_string_remove_start(char *str, int n);
  char *_utils_string_remove_end(char *str, int n);
  char *_utils_string_remove_at(char *str, int pos, int n);
  int _utils_string_has_char(char *str, const char c); // return count
  char *_utils_string_trim(char *str);
  
#endif
