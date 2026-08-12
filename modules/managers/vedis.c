// MODULES / MANAGERS / VEDIS.C

#include "vedis.h" // modules/managers/vedis.h
#include "../globals.h" // modules/globals.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
  #include <direct.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
#endif

#include "../utils/vedis.h"
#include "../_libs/cjson/cJSON.h"
#include "../utils/cjson.h"
#include "../utils/string.h"
#include "../utils/print.h"
#include "../utils/array.h"

/*
// Vedis Datastore Handle
VEDIS_APIEXPORT int vedis_open(vedis **ppStore,const char *zStorage);
VEDIS_APIEXPORT int vedis_config(vedis *pStore,int iOp,...);
VEDIS_APIEXPORT int vedis_close(vedis *pStore);

// Command Execution Interfaces
VEDIS_APIEXPORT int vedis_exec(vedis *pStore,const char *zCmd,int nLen);
VEDIS_APIEXPORT int vedis_exec_fmt(vedis *pStore,const char *zFmt,...);
VEDIS_APIEXPORT int vedis_exec_result(vedis *pStore,vedis_value **ppOut);

// Foreign Command Registar
VEDIS_APIEXPORT int vedis_register_command(vedis *pStore,const char *zName,int (*xCmd)(vedis_context *,int,vedis_value **),void *pUserdata);
VEDIS_APIEXPORT int vedis_delete_command(vedis *pStore,const char *zName);

// Raw Data Store/Fetch (http://vedis.org)
VEDIS_APIEXPORT int vedis_kv_store(vedis *pStore,const void *pKey,int nKeyLen,const void *pData,vedis_int64 nDataLen);
VEDIS_APIEXPORT int vedis_kv_append(vedis *pStore,const void *pKey,int nKeyLen,const void *pData,vedis_int64 nDataLen);
VEDIS_APIEXPORT int vedis_kv_store_fmt(vedis *pStore,const void *pKey,int nKeyLen,const char *zFormat,...);
VEDIS_APIEXPORT int vedis_kv_append_fmt(vedis *pStore,const void *pKey,int nKeyLen,const char *zFormat,...);
VEDIS_APIEXPORT int vedis_kv_fetch(vedis *pStore,const void *pKey,int nKeyLen,void *pBuf,vedis_int64 *pBufLen);    // vedis_int64 // in|out
VEDIS_APIEXPORT int vedis_kv_fetch_callback(vedis *pStore,const void *pKey,
	                    int nKeyLen,int (*xConsumer)(const void *,unsigned int,void *),void *pUserData);
VEDIS_APIEXPORT int vedis_kv_config(vedis *pStore,int iOp,...);
VEDIS_APIEXPORT int vedis_kv_delete(vedis *pStore,const void *pKey,int nKeyLen);

// Manual Transaction Manager
VEDIS_APIEXPORT int vedis_begin(vedis *pStore);
VEDIS_APIEXPORT int vedis_commit(vedis *pStore);
VEDIS_APIEXPORT int vedis_rollback(vedis *pStore);

// Utility interfaces
VEDIS_APIEXPORT int vedis_util_random_string(vedis *pStore,char *zBuf,unsigned int buf_size);
VEDIS_APIEXPORT unsigned int vedis_util_random_num(vedis *pStore);

// Call Context Key/Value Store Interfaces
VEDIS_APIEXPORT int vedis_context_kv_store(vedis_context *pCtx,const void *pKey,int nKeyLen,const void *pData,vedis_int64 nDataLen);
VEDIS_APIEXPORT int vedis_context_kv_append(vedis_context *pCtx,const void *pKey,int nKeyLen,const void *pData,vedis_int64 nDataLen);
VEDIS_APIEXPORT int vedis_context_kv_store_fmt(vedis_context *pCtx,const void *pKey,int nKeyLen,const char *zFormat,...);
VEDIS_APIEXPORT int vedis_context_kv_append_fmt(vedis_context *pCtx,const void *pKey,int nKeyLen,const char *zFormat,...);
VEDIS_APIEXPORT int vedis_context_kv_fetch(vedis_context *pCtx,const void *pKey,int nKeyLen,void *pBuf,vedis_int64 *pBufLen);    // vedis_int64 // in|out
VEDIS_APIEXPORT int vedis_context_kv_fetch_callback(vedis_context *pCtx,const void *pKey,
	                    int nKeyLen,int (*xConsumer)(const void *,unsigned int,void *),void *pUserData);
VEDIS_APIEXPORT int vedis_context_kv_delete(vedis_context *pCtx,const void *pKey,int nKeyLen);

// Command Execution Context Interfaces
VEDIS_APIEXPORT int vedis_context_throw_error(vedis_context *pCtx, int iErr, const char *zErr);
VEDIS_APIEXPORT int vedis_context_throw_error_format(vedis_context *pCtx, int iErr, const char *zFormat, ...);
VEDIS_APIEXPORT unsigned int vedis_context_random_num(vedis_context *pCtx);
VEDIS_APIEXPORT int vedis_context_random_string(vedis_context *pCtx, char *zBuf, int nBuflen);
VEDIS_APIEXPORT void * vedis_context_user_data(vedis_context *pCtx);
VEDIS_APIEXPORT int    vedis_context_push_aux_data(vedis_context *pCtx, void *pUserData);
VEDIS_APIEXPORT void * vedis_context_peek_aux_data(vedis_context *pCtx);
VEDIS_APIEXPORT void * vedis_context_pop_aux_data(vedis_context *pCtx);

// Setting The Return Value Of A Vedis Command
VEDIS_APIEXPORT int vedis_result_int(vedis_context *pCtx, int iValue);
VEDIS_APIEXPORT int vedis_result_int64(vedis_context *pCtx, vedis_int64 iValue);
VEDIS_APIEXPORT int vedis_result_bool(vedis_context *pCtx, int iBool);
VEDIS_APIEXPORT int vedis_result_double(vedis_context *pCtx, double Value);
VEDIS_APIEXPORT int vedis_result_null(vedis_context *pCtx);
VEDIS_APIEXPORT int vedis_result_string(vedis_context *pCtx, const char *zString, int nLen);
VEDIS_APIEXPORT int vedis_result_string_format(vedis_context *pCtx, const char *zFormat, ...);
VEDIS_APIEXPORT int vedis_result_value(vedis_context *pCtx, vedis_value *pValue);

// Extracting Vedis Commands Parameter/Return Values
VEDIS_APIEXPORT int vedis_value_to_int(vedis_value *pValue);
VEDIS_APIEXPORT int vedis_value_to_bool(vedis_value *pValue);
VEDIS_APIEXPORT vedis_int64 vedis_value_to_int64(vedis_value *pValue);
VEDIS_APIEXPORT double vedis_value_to_double(vedis_value *pValue);
VEDIS_APIEXPORT const char * vedis_value_to_string(vedis_value *pValue, int *pLen);

// Dynamically Typed Value Object Query Interfaces
VEDIS_APIEXPORT int vedis_value_is_int(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_float(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_bool(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_string(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_null(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_numeric(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_scalar(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_is_array(vedis_value *pVal);

// Populating dynamically Typed Objects
VEDIS_APIEXPORT int vedis_value_int(vedis_value *pVal, int iValue);
VEDIS_APIEXPORT int vedis_value_int64(vedis_value *pVal, vedis_int64 iValue);
VEDIS_APIEXPORT int vedis_value_bool(vedis_value *pVal, int iBool);
VEDIS_APIEXPORT int vedis_value_null(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_double(vedis_value *pVal, double Value);
VEDIS_APIEXPORT int vedis_value_string(vedis_value *pVal, const char *zString, int nLen);
VEDIS_APIEXPORT int vedis_value_string_format(vedis_value *pVal, const char *zFormat, ...);
VEDIS_APIEXPORT int vedis_value_reset_string_cursor(vedis_value *pVal);
VEDIS_APIEXPORT int vedis_value_release(vedis_value *pVal);

// On-demand Object Value Allocation
VEDIS_APIEXPORT vedis_value * vedis_context_new_scalar(vedis_context *pCtx);
VEDIS_APIEXPORT vedis_value * vedis_context_new_array(vedis_context *pCtx);
VEDIS_APIEXPORT void vedis_context_release_value(vedis_context *pCtx, vedis_value *pValue);

// Working with Vedis Arrays
VEDIS_APIEXPORT vedis_value * vedis_array_fetch(vedis_value *pArray,unsigned int index);
VEDIS_APIEXPORT int vedis_array_walk(vedis_value *pArray, int (*xWalk)(vedis_value *, void *), void *pUserData);
VEDIS_APIEXPORT int vedis_array_insert(vedis_value *pArray,vedis_value *pValue);
VEDIS_APIEXPORT unsigned int vedis_array_count(vedis_value *pArray);
VEDIS_APIEXPORT int vedis_array_reset(vedis_value *pArray);
VEDIS_APIEXPORT vedis_value * vedis_array_next_elem(vedis_value *pArray);

// Global Library Management Interfaces
VEDIS_APIEXPORT int vedis_lib_init(void);
VEDIS_APIEXPORT int vedis_lib_config(int nConfigOp, ...);
VEDIS_APIEXPORT int vedis_lib_shutdown(void);
VEDIS_APIEXPORT int vedis_lib_is_threadsafe(void);
VEDIS_APIEXPORT const char * vedis_lib_version(void);
VEDIS_APIEXPORT const char * vedis_lib_signature(void);
VEDIS_APIEXPORT const char * vedis_lib_ident(void);
VEDIS_APIEXPORT const char * vedis_lib_copyright(void);
*/

void _managers_vedis_handle_rc(int rc) {
  _utils_printf(rc == 0 ? NULL : "err", "VEDIS %s, code: %d\n", _utils_vedis_rc_to_str(rc), rc);
  switch (rc) {
    case SXERR_MEM:
    case SXERR_IO:
    case SXERR_PERM:
    case SXERR_OS:
    case SXERR_CORRUPT:
    case VEDIS_LOCKERR:
    case ERR_MANAGERS_VEDIS_EXEC_POINTER:
    case ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK:
    case ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK:
    case ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER:
    case ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK:
    case ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK:
      stop_requested = 1;
      break;
    case SXERR_TIMEOUT:
      restart_requested = 1;
      stop_requested = 1;
      break;
  }
}

VedisManager *_managers_vedis_manager_get(VedisManagers *vedis_managers, const char *name) {
  for (int i = 0; i < vedis_managers->count; i++) {
    if (strcmp(vedis_managers->managers[i].name, name) == 0) {
      return &vedis_managers->managers[i];
    }
  }
  return NULL;
}

int _managers_vedis_list(VedisManagers *vedis_managers) {
  _utils_printf(NULL, "VEDIS DB:\n");
  for (int i = 0; i < vedis_managers->count; i++) {
    _utils_printf(NULL, "  %d: %s\n", i, vedis_managers->managers[i].name);
  }
  return VEDIS_OK;
}

int _managers_vedis_store_add(VedisManagers *vedis_managers, const char *storage_path, const char *name) {
  if (!vedis_managers) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_INIT_POINTER); return ERR_MANAGERS_VEDIS_INIT_POINTER; } // change/add code
  
  VedisManager *new_vedis_manager = &vedis_managers->managers[vedis_managers->count];
  
  if (pthread_mutex_init(&new_vedis_manager->lock, NULL) != 0) {
    _globals_app_rc_log(ERR_MANAGERS_VEDIS_INIT_MUTEX);
    return ERR_MANAGERS_VEDIS_INIT_MUTEX;
  }
  
  int rc;
  
  if (storage_path && storage_path != ":mem:") {
    char buffer[512];
    int has_char_count = _utils_string_has_char(storage_path, '/');
    char *array_storage_path[has_char_count + 1];
    int array_storage_path_len = 0;
    
    if (has_char_count > 0) {
      char *str = strdup(storage_path);
      if (!str) { 
        for (int i = 0; i < has_char_count + 1; i++) {
          free(array_storage_path[i]);
        }
        return -1;
      }
      
      char *token = strtok(str, "/");
      
      while (token != NULL && array_storage_path_len <= has_char_count) {
        array_storage_path[array_storage_path_len] = strdup(token); 
        
        if (array_storage_path[array_storage_path_len]) { array_storage_path_len++; }
        token = strtok(NULL, "/");
      }
      free(str);
      
      for (int i = 0; i < array_storage_path_len; i++) {
        if (strncmp(array_storage_path[i], "$", 1) == 0) {
          char *env = getenv(_utils_string_remove_start(array_storage_path[i], 1));
          if (env) {
            free(array_storage_path[i]);
            array_storage_path[i] = strdup(env);
          }
        }
      }
      
      for (int i = array_storage_path_len-1; i >= 1; i--) {
        _utils_array_to_stringbuffer(array_storage_path, array_storage_path_len-i, buffer, sizeof(buffer), 0, 0, "/");
        #ifdef _WIN32
          _mkdir(buffer);
        #else
          mkdir(buffer, 0755);
        #endif
      }
      
      _utils_printf(NULL, "VEDIS MANAGER OPEN: %s -> %s\n", name, _utils_array_to_stringbuffer(array_storage_path, array_storage_path_len, buffer, sizeof(buffer), 0, 0, "/"));
      rc = vedis_open(&new_vedis_manager->pStore, buffer);
    }
    
    for (int i = 0; i < array_storage_path_len; i++) {
      free(array_storage_path[i]);
    }
  } else {
    _utils_printf(NULL, "VEDIS MANAGER OPEN: %s -> %s\n", name, ":mem:");
    rc = vedis_open(&new_vedis_manager->pStore, ":mem:");
  }
  
  if (rc != VEDIS_OK) {
      _globals_app_rc_log(ERR_MANAGERS_VEDIS_INIT_OPEN);
      pthread_mutex_destroy(&new_vedis_manager->lock);
      return ERR_MANAGERS_VEDIS_INIT_OPEN;
  }
  
  new_vedis_manager->name = name;
  new_vedis_manager->path = storage_path ? storage_path : ":mem:";
  vedis_managers->count++;
  
  _globals_app_rc_log(OK_MANAGERS_VEDIS_INIT);
  return OK_MANAGERS_VEDIS_INIT;
}

int _managers_vedis_close(VedisManagers *vedis_managers) {
  if (!vedis_managers) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_CLOSE_POINTER); return ERR_MANAGERS_VEDIS_CLOSE_POINTER; }
  
  for (int index = vedis_managers->count-1; index >= 0; index--) {
  
    VedisManager *current_vedis_manager = &vedis_managers->managers[index];
    
    _utils_printf(NULL, "VEDIS MANAGER CLOSE: %s -> %s\n", current_vedis_manager->name, current_vedis_manager->path);
    
    if (pthread_mutex_lock(&current_vedis_manager->lock) != 0) {
      _globals_app_rc_log(ERR_MANAGERS_VEDIS_CLOSE_MUTEX_LOCK);
      return ERR_MANAGERS_VEDIS_CLOSE_MUTEX_LOCK;
    }
    int rc;
    
    if (current_vedis_manager->pStore) {
      rc = vedis_close(current_vedis_manager->pStore);
      current_vedis_manager->pStore = NULL;
    }
    
    if (rc != VEDIS_OK) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_CLOSE_POINTER_PSTORE); return ERR_MANAGERS_VEDIS_CLOSE_POINTER_PSTORE; }
    if (pthread_mutex_unlock(&current_vedis_manager->lock) != 0) {
      _globals_app_rc_log(ERR_MANAGERS_VEDIS_CLOSE_MUTEX_UNLOCK);
      return ERR_MANAGERS_VEDIS_CLOSE_MUTEX_UNLOCK;
    }
    if (pthread_mutex_destroy(&current_vedis_manager->lock) != 0) {
      _globals_app_rc_log(ERR_MANAGERS_VEDIS_CLOSE_MUTEX_DESTROY);
      return ERR_MANAGERS_VEDIS_CLOSE_MUTEX_DESTROY;
    }
    
  }
  
  free(vedis_managers->managers);
  
  _globals_app_rc_log(OK_MANAGERS_VEDIS_CLOSE);
  return OK_MANAGERS_VEDIS_CLOSE;
}

int _managers_vedis_exec(VedisManager *vedis_manager, char *zCmd, int nLen, const char *as_fmt, ...) {
  if (stop_requested || restart_requested) { return OK_MANAGERS_RESTART_STOP_RETRY; }
  if (!vedis_manager || !vedis_manager->pStore) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_POINTER); _managers_vedis_handle_rc(ERR_MANAGERS_VEDIS_EXEC_POINTER); return ERR_MANAGERS_VEDIS_EXEC_POINTER; }
  if (!zCmd) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_CMD); return ERR_MANAGERS_VEDIS_EXEC_CMD; }
  if (pthread_mutex_lock(&vedis_manager->lock) != 0) {
    _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK);
    _managers_vedis_handle_rc(ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK);
    return ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK;
  }
  int rc;
  char *zCmdFmt;
  
  if (as_fmt) {
    va_list args;
    va_start(args, as_fmt);
    zCmdFmt = _utils_string_vaprintf_to_string(zCmd, args);
    va_end(args);
    if (!zCmdFmt) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_CMD_FMT); return ERR_MANAGERS_VEDIS_EXEC_CMD_FMT; }
  }
  
  rc = vedis_exec(vedis_manager->pStore, as_fmt ? zCmdFmt : zCmd, nLen ? nLen : -1);
  if (as_fmt) { free(zCmdFmt); }
  
  if (rc != VEDIS_OK) { _managers_vedis_handle_rc(rc); }
  //_managers_vedis_handle_rc(rc);
  if (pthread_mutex_unlock(&vedis_manager->lock) != 0) {
    _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK);
    _managers_vedis_handle_rc(ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK);
    return ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK;
  }
  return rc; // keep vedis rc
}

int _managers_vedis_exec_result(VedisManager *vedis_manager, VedisValue **value) {
  if (stop_requested || restart_requested) { return OK_MANAGERS_RESTART_STOP_RETRY; }
  if (!vedis_manager || !vedis_manager->pStore) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER); _managers_vedis_handle_rc(ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER); return ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER; }
  if (pthread_mutex_lock(&vedis_manager->lock) != 0) {
    _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK);
    _managers_vedis_handle_rc(ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK);
    return ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK;
  }
  
  vedis_value *pResult;
  int rc = vedis_exec_result(vedis_manager->pStore, &pResult);
  if (rc == VEDIS_OK) {
    *value = _utils_vedis_parse_value(pResult);
  }
  
  if (rc != VEDIS_OK) { _managers_vedis_handle_rc(rc); }
  //_managers_vedis_handle_rc(rc);
  if (pthread_mutex_unlock(&vedis_manager->lock) != 0) {
    _globals_app_rc_log(ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK);
    _managers_vedis_handle_rc(ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK);
    return ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK;
  }
  return rc; // keep vedis rc
}

#if defined(PLOUCKY_ENABLE_VEDIS_CMD)
  int _managers_vedis_exec_cmdcli(VedisManagers *vedis_managers, const char *cmd) {
    char *buffer = strdup(cmd);
    if (!buffer) { return -1; }
    int rc;
    
    _utils_string_remove_start(buffer, strlen("/vedis "));
    VedisManager *vedis_manager = NULL;
    int has_char_count = _utils_string_has_char(cmd, ':');
    char *array_cmd[has_char_count + 1];
    int array_cmd_len = 0;
    
    if (has_char_count > 0) {
      char *str = strdup(buffer);
      if (!str) { 
        for (int i = 0; i < has_char_count + 1; i++) {
          free(array_cmd[i]);
        }
        free(buffer);
        return -1;
      }
      
      char *token = strtok(str, ":");
      
      while (token != NULL && array_cmd_len <= has_char_count) {
        if (array_cmd_len == 0) { _utils_string_remove_start(buffer, strlen(token)+1); }
        array_cmd[array_cmd_len] = strdup(_utils_string_trim(token)); 
        
        if (array_cmd[array_cmd_len]) { array_cmd_len++; }
        token = strtok(NULL, ":");
      }
      free(str);
      
      /*
      for (int i = 0; i < array_cmd_len; i++) {
        _utils_printf(NULL, "vedis cmd [%d]: %s\n", i, array_cmd[i]);
        _utils_printf(NULL, "vedis cmd len [%d]: %d\n", i, strlen(array_cmd[i]));
      }
      _utils_printf(NULL, "vedis cmd buffer: %s\n", buffer);
      */
      
      vedis_manager = _managers_vedis_manager_get(vedis_managers, array_cmd[0]);
      if (vedis_manager) { _utils_string_trim(buffer); }
    } else {
      _utils_string_trim(buffer);
      if (strcmp(buffer, "list") == 0) { rc = _managers_vedis_list(vedis_managers); }
      else { vedis_manager = &vedis_managers->managers[0]; }
    }
    
    if (vedis_manager) {
      rc = _managers_vedis_exec(vedis_manager, buffer, -1, NULL);
      if (rc == VEDIS_OK) {
        VedisValue *value;
        rc = _managers_vedis_exec_result(vedis_manager, &value);
        if (rc == VEDIS_OK) {
          if (_utils_vedis_parse_value_isok(value)) {
            _utils_vedis_print_value(value);
            _utils_vedis_value_destroy(value);
          }
        }
      }
    } else if (has_char_count > 0 && !vedis_manager) {
      _utils_printf(NULL, "Unknown VEDIS DB: %s\n", array_cmd[0]);
      rc = SXERR_UNKNOWN;
    }
    
    for (int i = 0; i < array_cmd_len; i++) {
      free(array_cmd[i]);
    }
    free(buffer);
    return rc;
  }
#endif

void vedis_test(VedisManagers *vedis_managers) {
  int rc;
  
  VedisManager *vedis_manager = _managers_vedis_manager_get(vedis_managers, "memory");
  
  _utils_printf(NULL, "1. Test INT:\n");
  _managers_vedis_exec(vedis_manager, "SET my_int 42", -1, NULL);
  _managers_vedis_exec(vedis_manager, "GET my_int", -1, NULL);
  //
  VedisValue *value;
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }


  _utils_printf(NULL, "1.1. Test INT natif (INCR):\n");
  _managers_vedis_exec(vedis_manager, "INCR my_incr", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }


  _utils_printf(NULL, "2. Test FLOAT:\n");
  _managers_vedis_exec(vedis_manager, "SET my_float 3.14159", -1, NULL);
  _managers_vedis_exec(vedis_manager, "GET my_float", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }
  

  _utils_printf(NULL, "3. Test BOOL:\n");
  _managers_vedis_exec(vedis_manager, "SET my_bool true", -1, NULL);
  _managers_vedis_exec(vedis_manager, "GET my_bool", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }


  _utils_printf(NULL, "4. Test STRING:\n");
  _managers_vedis_exec(vedis_manager, "SET my_str \"Hello World\"", -1, NULL);
  _managers_vedis_exec(vedis_manager, "GET my_str", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }
  

  _utils_printf(NULL, "5. Test HGETALL ([key, value, key, value]):\n");
  _managers_vedis_exec(vedis_manager, "HMSET config pid %d user %s", -1, "FMT", 1024, "alice");
  _managers_vedis_exec(vedis_manager, "HGETALL config", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }


  _utils_printf(NULL, "6. Test NULL:\n");
  _managers_vedis_exec(vedis_manager, "GET nothing", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }


  _utils_printf(NULL, "7. Test JSON (Brut) :\n");
  _managers_vedis_exec(vedis_manager, "SET my_json '{\"id\":1, \"actif\":true}'", -1, NULL);
  _managers_vedis_exec(vedis_manager, "GET my_json", -1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }
  
  
  _utils_printf(NULL, "8. Test HVALS (without keys):\n");
  _managers_vedis_exec(vedis_manager,"HVALS config",-1, NULL);
  //
  rc = _managers_vedis_exec_result(vedis_manager, &value);
  if (_utils_vedis_parse_value_isok(value)) {
    _utils_vedis_print_value(value);
    _utils_vedis_value_destroy(value);
  }
  
  
  _utils_printf(NULL, "9. Test cJSON (MERGE):\n");
  char *json_string = "{ \"user\": { \"profile\": { \"name\": \"Alice\", \"age\": 30 }}}";
  cJSON *json_string_parsed = cJSON_Parse(json_string);
  char *json_string2 = "{ \"user\": { \"profile2\": { \"name\": \"Bob\", \"age\": 40 }}}";
  cJSON *json_string_parsed2 = cJSON_Parse(json_string2);
  _utils_cjson_merge_objects(json_string_parsed, json_string_parsed2);
  //
  cJSON *json_user = cJSON_GetObjectItem(json_string_parsed, "user");
  cJSON *json_user_profile = cJSON_GetObjectItem(json_user, "profile2");
  char *json_print_parsed = cJSON_PrintUnformatted(json_string_parsed);
  char *json_print_user = cJSON_PrintUnformatted(json_user);
  char *json_print_user_profile = cJSON_PrintUnformatted(json_user_profile);
  //
  _utils_printf(NULL, " -> json_string (merged) %s\n", json_print_parsed);
  _utils_printf(NULL, " -> json_string (user:) %s\n", json_print_user);
  _utils_printf(NULL, " -> json_string (user:profile2:) %s\n", json_print_user_profile);
  cJSON_Delete(json_string_parsed);
  cJSON_Delete(json_string_parsed2);
  //cJSON_Delete(json_user);
  //cJSON_Delete(json_user_profile); // double free, do not free Get..
  free(json_print_parsed);
  free(json_print_user);
  free(json_print_user_profile);
  
  //https://vedis.symisc.net/commands.html
}
