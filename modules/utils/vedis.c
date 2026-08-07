// MODULES / UTILS / VEDIS.C

#include "vedis.h" // modules/utils/vedis.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h" // modules/utils/print.h



const char *_utils_vedis_rc_to_str(int rc) {
  switch (rc) {
    case SXRET_OK:             return "RC OK";
    case SXERR_MEM:            return "RC Out of memory";
    case SXERR_IO:             return "RC IO error";
    case SXERR_EMPTY:          return "RC Empty field";
    case SXERR_LOCKED:         return "RC Locked operation";
    case SXERR_ORANGE:         return "RC Out of range value";
    case SXERR_NOTFOUND:       return "RC Item not found";
    case SXERR_LIMIT:          return "RC Limit reached";
    case SXERR_MORE:           return "RC Need more input";
    case SXERR_INVALID:        return "RC Invalid parameter";
    case SXERR_ABORT:          return "RC User callback request an operation abort";
    case SXERR_EXISTS:         return "RC Item exists";
    case SXERR_SYNTAX:         return "RC Syntax error";
    case SXERR_UNKNOWN:        return "RC Unknown error";
    case SXERR_BUSY:           return "RC Busy operation";
    case SXERR_OVERFLOW:       return "RC Stack or buffer overflow";
    case SXERR_WILLBLOCK:      return "RC Operation will block";
    case SXERR_NOTIMPLEMENTED: return "RC Operation not implemented";
    case SXERR_EOF:            return "RC End of input";
    case SXERR_PERM:           return "RC Permission error";
    case SXERR_NOOP:           return "RC No-op";	
    case SXERR_FORMAT:         return "RC Invalid format";
    case SXERR_NEXT:           return "RC Not an error";
    case SXERR_OS:             return "RC System call return an error";
    case SXERR_CORRUPT:        return "RC Corrupted pointer";
    case SXERR_CONTINUE:       return "RC Not an error: Operation in progress";
    case SXERR_NOMATCH:        return "RC No match";
    case SXERR_RESET:          return "RC Operation reset";
    case SXERR_DONE:           return "RC Not an error";
    case SXERR_SHORT:          return "RC Buffer too short";
    case SXERR_PATH:           return "RC Path error";
    case SXERR_TIMEOUT:        return "RC Timeout";
    case SXERR_BIG:            return "RC Too big for processing";
    case SXERR_RETRY:          return "RC Retry your call";
    case SXERR_IGNORE:         return "RC Ignore";
    case VEDIS_FULL:           return "RC Full database (unlikely)";
    case VEDIS_CANTOPEN:       return "RC Unable to open the database file";
    case VEDIS_READ_ONLY:      return "RC Read only Key/Value storage engine";
    case VEDIS_LOCKERR:        return "RC Locking protocol error";
    default: return "INVALID RC";
  }
}
const char *_utils_vedis_valuetype_to_str(int type) {
  switch (type) {
    case TYPE_ARRAY:           return "TYPE_ARRAY";
    case TYPE_BOOL:            return "TYPE_BOOL";
    case TYPE_INT:             return "TYPE_INT";
    case TYPE_FLOAT:           return "TYPE_FLOAT";
    case TYPE_JSON:            return "TYPE_JSON";
    case TYPE_STRING:          return "TYPE_STRING";
    default: return "INVALID TYPE";
  }
}

//
void _utils_vedis_value_destroy(VedisValue *value); // Function prototype before declaration
void _utils_vedis_value_destroy(VedisValue *value) {
  if (!value) return;
  if (value->_string_ptr) { free(value->_string_ptr); value->_string_ptr = NULL; }
  if (value->type == TYPE_ARRAY && value->_array_ptr) {
    for (int i = 0; i < value->_array_len; i++) {
      _utils_vedis_value_destroy(value->_array_ptr[i]); // <-
    }
    free(value->_array_ptr); value->_array_ptr = NULL;
  }
  memset(value, 0, sizeof(VedisValue));
  free(value); value = NULL;
}
int _utils_vedis_parse_value_isok(VedisValue *value) {
  if ( !value || !value->_string_ptr || (value->type == TYPE_ARRAY && !value->_array_ptr) ) {
    _utils_vedis_value_destroy(value);
    return 0;
  }
  return 1;
}
typedef struct { VedisValue **dest_array; int index; } FillCtx;
int count_callback(vedis_value *pItem, void *pUserData) {
  int *count = (int *)pUserData; (*count)++; return VEDIS_OK;
}
VedisValue *_utils_vedis_parse_value(vedis_value *pVal); // Function prototype before declaration
int fill_callback(vedis_value *pItem, void *pUserData) {
  FillCtx *ctx = (FillCtx *)pUserData;
  VedisValue *new_val = _utils_vedis_parse_value(pItem); // <-
  if (_utils_vedis_parse_value_isok(new_val)) {
    ctx->dest_array[ctx->index] = new_val;
    ctx->index++;
  }
  return VEDIS_OK;
}
VedisValue *_utils_vedis_parse_value(vedis_value *pVal) {
  VedisValue *copy = calloc(1, sizeof(VedisValue));
  if (!copy || !pVal || vedis_value_is_null(pVal)) { _utils_vedis_value_destroy(copy); return NULL; }
  
  if (vedis_value_is_array(pVal)) {
    int count = 0;        
    vedis_array_walk(pVal, count_callback, &count);
    copy->_array_len = count;          
    if (count > 0) {
      copy->_array_ptr = calloc(count, sizeof(VedisValue *));
      if (copy->_array_ptr) {
        if (copy->_array_ptr) {
          FillCtx ctx;
          ctx.dest_array = copy->_array_ptr;
          ctx.index = 0;
          vedis_array_walk(pVal, fill_callback, &ctx);
        }
      } else {
        _utils_vedis_value_destroy(copy);
        return NULL;
      }
    }
    copy->type = TYPE_ARRAY;
  } else if (vedis_value_is_bool(pVal)) {
    copy->type = TYPE_BOOL;
    copy->_bool = vedis_value_to_bool(pVal);
  } else if (vedis_value_is_int(pVal)) {
    copy->type = TYPE_INT;
    copy->_int = vedis_value_to_int(pVal);
  } else if (vedis_value_is_float(pVal)) {
    copy->type = TYPE_FLOAT;
    copy->_double = vedis_value_to_double(pVal);
  } else if (vedis_value_is_string(pVal)) {
    int intlen_str;
    const char *str = vedis_value_to_string(pVal, &intlen_str);
    if (strcmp(str, "true") == 0) {
      copy->type = TYPE_BOOL;
      copy->_bool = 1;
    } else if (strcmp(str, "false") == 0) {
      copy->type = TYPE_BOOL;
      copy->_bool = 0;
    } else if (vedis_value_is_numeric(pVal)) {
      if (strchr(str, '.') != NULL) {
        copy->type = TYPE_FLOAT;
        double val = vedis_value_to_double(pVal);
        copy->_double = val;
      } else {
        copy->type = TYPE_INT;
        int val = vedis_value_to_int(pVal);
        copy->_int = val;
      }
    } else {
      if (str[0] == '{' && str[intlen_str-1] == '}') {
        copy->type = TYPE_JSON;
      } else if (str[0] == '[' && str[intlen_str-1] == ']') {
        copy->type = TYPE_ARRAY;
      } else {
        copy->type = TYPE_STRING;
      }
    }
  } else {
    _utils_vedis_value_destroy(copy);
    return NULL;
  }
  
  int intlen_strdefault;
  const char *strdefault = vedis_value_to_string(pVal, &intlen_strdefault);
  if (strdefault && intlen_strdefault >= 0) {
    copy->_string_ptr = (char *)malloc(intlen_strdefault + 1);
    if (copy->_string_ptr) {
      memcpy(copy->_string_ptr, strdefault, intlen_strdefault);
      copy->_string_ptr[intlen_strdefault] = '\0';
      copy->_string_len = intlen_strdefault; 
    } else {
      _utils_vedis_value_destroy(copy);
      return NULL;
    }
  } else {
    _utils_vedis_value_destroy(copy);
    return NULL;
  }
  
  return copy;
}

void _utils_vedis_rc_log(int rc) {
  _utils_printf(rc >= 0 ? NULL : "err", "VEDIS_RC -> %s:%d\n", _utils_vedis_rc_to_str(rc), rc);
}

void _utils_vedis_print_value(VedisValue *value) {
  _utils_printf(NULL, " -> %s:%d\n", _utils_vedis_valuetype_to_str(value->type), value->type);
  _utils_printf(NULL, "     -> (as string) %s\n", value->_string_ptr);
  if (value->type == TYPE_INT) {
    _utils_printf(NULL, "     -> (as type) %d\n", value->_int);
  }
  if (value->type == TYPE_FLOAT) {
    _utils_printf(NULL, "     -> (as type) %f\n", value->_double);
  }
  if (value->type == TYPE_BOOL) {
    _utils_printf(NULL, "     -> (as type) %d\n", value->_bool);
  }
  
  if (value->type == TYPE_ARRAY) {
    for (int index = 0; index < value->_array_len; index++) {
      _utils_printf(NULL, "  ");
      _utils_vedis_print_value(value->_array_ptr[index]);
    }
  }
} 


