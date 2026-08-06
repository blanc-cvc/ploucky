// MODULES / UTILS / VEDIS.H

#ifndef UTILS_VEDIS_H
  #define UTILS_VEDIS_H

  #include "../_libs/vedis/vedis.h"

  const char *_utils_vedis_rc_to_str(int rc);
  const char *_utils_vedis_valuetype_to_str(int type);

  typedef enum {
      TYPE_ARRAY,
      TYPE_BOOL,
      TYPE_INT,
      TYPE_FLOAT,
      TYPE_JSON,
      TYPE_STRING,
  } VedisType;

  struct VedisValue;

  typedef struct VedisValue {
      VedisType type;
      int _int;
      int _bool;
      double _double;
      char *_string_ptr;
      int _string_len;
      struct VedisValue **_array_ptr;
      int _array_len;
  } VedisValue;

  void _utils_vedis_value_destroy(VedisValue *value);
  int _utils_vedis_parse_value_isok(VedisValue *value);
  VedisValue *_utils_vedis_parse_value(vedis_value *pVal);
  void _utils_vedis_rc_log(int rc);
  void _utils_vedis_print_value(VedisValue *value);
  
#endif
