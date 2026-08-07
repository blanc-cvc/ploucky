// MODULES / GLOBALS.H

#ifndef GLOBALS_H
#define GLOBALS_H

  #include <signal.h>
  #include <stdio.h>
  #include "_libs/linenoise/linenoise.h"
  #include "utils/print.h"
  
  #define TERMINAL_STYLE_RED     "\033[31m"
  #define TERMINAL_STYLE_GREEN   "\033[32m"
  #define TERMINAL_STYLE_YELLOW  "\033[33m"
  #define TERMINAL_STYLE_BLUE    "\033[34m"
  #define TERMINAL_STYLE_RESET   "\033[0m"
  #define TERMINAL_STYLE_BOLD    "\033[1m"

  typedef enum {
    //
    ERR_MANAGERS_VEDIS_CALLOC =                        -1000,
    
     OK_MANAGERS_VEDIS_INIT =                           1001,
    ERR_MANAGERS_VEDIS_INIT_POINTER =                  -1002,
    ERR_MANAGERS_VEDIS_INIT_MUTEX =                    -1003,
    ERR_MANAGERS_VEDIS_INIT_OPEN =                     -1004,
    
     OK_MANAGERS_VEDIS_CLOSE =                          1005,
    ERR_MANAGERS_VEDIS_CLOSE_POINTER =                 -1006,
    ERR_MANAGERS_VEDIS_CLOSE_POINTER_PSTORE =          -1007,
    ERR_MANAGERS_VEDIS_CLOSE_MUTEX_LOCK =              -1008,
    ERR_MANAGERS_VEDIS_CLOSE_MUTEX_UNLOCK =            -1009,
    ERR_MANAGERS_VEDIS_CLOSE_MUTEX_DESTROY =           -1010,
    
    ERR_MANAGERS_VEDIS_EXEC_POINTER =                  -1011,
    ERR_MANAGERS_VEDIS_EXEC_CMD =                      -1012,
    ERR_MANAGERS_VEDIS_EXEC_CMD_FMT =                  -1013,
    ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK =               -1014,
    ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK =             -1015,
    
    ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER =           -1016,
    ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK =        -1017,
    ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK =      -1018,
    
    //
    ERR_MANAGERS_HTTP_CALLOC =                         -2000,
    ERR_MANAGERS_HTTP_PORT_UNAVAILABLE =               -2001,
    
     OK_MANAGERS_HTTP_INIT =                            2002,
    ERR_MANAGERS_HTTP_INIT =                           -2002,
    ERR_MANAGERS_HTTP_INIT_POINTER =                   -2003,
    ERR_MANAGERS_HTTP_INIT_CONTEXT =                   -2004,
    
     OK_MANAGERS_HTTP_CLOSE =                           2005,
    ERR_MANAGERS_HTTP_CLOSE =                          -2005,
    
    ERR_MANAGERS_HTTP_REQUEST_VEDIS_POINTER =          -2006,
    ERR_MANAGERS_HTTP_REQUEST_VEDIS_PARSE_VALUE =      -2007,
    ERR_MANAGERS_HTTP_REQUEST_METHOD_NOT_HANDLED =     -2008,
    ERR_MANAGERS_HTTP_REQUEST_URI_NOT_HANDLED =        -2009,
    
    //
     OK_MANAGERS_RESTART_STOP_RETRY =                   3000,
       
  } appReturnCode;
  
  static inline const char *_globals_app_rc_to_string(int rc) {
    switch (rc) {
      case ERR_MANAGERS_VEDIS_CALLOC:                     return "ERR_MANAGERS_VEDIS_CALLOC";
      
      case  OK_MANAGERS_VEDIS_INIT:                       return "OK_MANAGERS_VEDIS_INIT";
      case ERR_MANAGERS_VEDIS_INIT_POINTER:               return "ERR_MANAGERS_VEDIS_INIT_POINTER";
      case ERR_MANAGERS_VEDIS_INIT_MUTEX:                 return "ERR_MANAGERS_VEDIS_INIT_MUTEX";
      case ERR_MANAGERS_VEDIS_INIT_OPEN:                  return "ERR_MANAGERS_VEDIS_INIT_OPEN";
      
      case  OK_MANAGERS_VEDIS_CLOSE:                      return "OK_MANAGERS_VEDIS_CLOSE";
      case ERR_MANAGERS_VEDIS_CLOSE_POINTER:              return "ERR_MANAGERS_VEDIS_CLOSE_POINTER";
      case ERR_MANAGERS_VEDIS_CLOSE_POINTER_PSTORE:       return "ERR_MANAGERS_VEDIS_CLOSE_POINTER_PSTORE";
      case ERR_MANAGERS_VEDIS_CLOSE_MUTEX_LOCK:           return "ERR_MANAGERS_VEDIS_CLOSE_MUTEX_LOCK";
      case ERR_MANAGERS_VEDIS_CLOSE_MUTEX_UNLOCK:         return "ERR_MANAGERS_VEDIS_CLOSE_MUTEX_UNLOCK";
      case ERR_MANAGERS_VEDIS_CLOSE_MUTEX_DESTROY:        return "ERR_MANAGERS_VEDIS_CLOSE_MUTEX_DESTROY";
      
      case ERR_MANAGERS_VEDIS_EXEC_POINTER:               return "ERR_MANAGERS_VEDIS_EXEC_POINTER";
      case ERR_MANAGERS_VEDIS_EXEC_CMD:                   return "ERR_MANAGERS_VEDIS_EXEC_CMD";
      case ERR_MANAGERS_VEDIS_EXEC_CMD_FMT:               return "ERR_MANAGERS_VEDIS_EXEC_CMD_FMT";
      case ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK:            return "ERR_MANAGERS_VEDIS_EXEC_MUTEX_LOCK";
      case ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK:          return "ERR_MANAGERS_VEDIS_EXEC_MUTEX_UNLOCK";
      
      case ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER:        return "ERR_MANAGERS_VEDIS_EXEC_RESULT_POINTER";
      case ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK:     return "ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_LOCK";
      case ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK:   return "ERR_MANAGERS_VEDIS_EXEC_RESULT_MUTEX_UNLOCK";
      
      //
      case ERR_MANAGERS_HTTP_CALLOC:                      return "ERR_MANAGERS_HTTP_CALLOC";
      case ERR_MANAGERS_HTTP_PORT_UNAVAILABLE:            return "ERR_MANAGERS_HTTP_PORT_UNAVAILABLE";
      
      case  OK_MANAGERS_HTTP_INIT:                        return "OK_MANAGERS_HTTP_INIT";
      case ERR_MANAGERS_HTTP_INIT:                        return "ERR_MANAGERS_HTTP_INIT";
      case ERR_MANAGERS_HTTP_INIT_POINTER:                return "ERR_MANAGERS_HTTP_INIT_POINTER";
      case ERR_MANAGERS_HTTP_INIT_CONTEXT:                return "ERR_MANAGERS_HTTP_INIT_CONTEXT";
      
      case  OK_MANAGERS_HTTP_CLOSE:                       return "OK_MANAGERS_HTTP_CLOSE";
      case ERR_MANAGERS_HTTP_CLOSE:                       return "ERR_MANAGERS_HTTP_CLOSE";
      
      //
      case ERR_MANAGERS_HTTP_REQUEST_VEDIS_POINTER:       return "ERR_MANAGERS_HTTP_REQUEST_VEDIS_POINTER";
      case ERR_MANAGERS_HTTP_REQUEST_VEDIS_PARSE_VALUE:   return "ERR_MANAGERS_HTTP_REQUEST_VEDIS_PARSE_VALUE";
      case ERR_MANAGERS_HTTP_REQUEST_METHOD_NOT_HANDLED:  return "ERR_MANAGERS_HTTP_REQUEST_METHOD_NOT_HANDLED";
      case ERR_MANAGERS_HTTP_REQUEST_URI_NOT_HANDLED:     return "ERR_MANAGERS_HTTP_REQUEST_URI_NOT_HANDLED";
      
      //
      case  OK_MANAGERS_RESTART_STOP_RETRY:               return "OK_MANAGERS_RESTART_STOP_RETRY";
      
      default: return "UNDEFINED";
      
    }
  }
  
  static inline void _globals_app_rc_log(int rc) {
    if (rc >= 0) { printf("%s", TERMINAL_STYLE_GREEN); }
    _utils_printf(rc >= 0 ? NULL : "err", "APP_RC -> %s:%d\n", _globals_app_rc_to_string(rc), rc);
    if (rc >= 0) { printf("%s", TERMINAL_STYLE_RESET); }
  }

  extern volatile sig_atomic_t stop_requested;
  extern sig_atomic_t restart_requested;
  extern struct linenoiseState linenoise_state;

#endif
