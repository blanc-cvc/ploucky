// MODULES / MANAGERS / VEDIS.H

#ifndef MANAGERS_VEDIS_H
  #define MANAGERS_VEDIS_H

  #include "../_libs/vedis/vedis.h"
  #include <pthread.h>
  #include <stdarg.h>
  #include "../utils/vedis.h"

  typedef struct {
      vedis *pStore;
      pthread_mutex_t lock;
  } VedisManager;

  void _managers_vedis_handle_rc(int rc);
  int _managers_vedis_init(VedisManager *vedis_manager, const char *storage_path);
  int _managers_vedis_close(VedisManager *vedis_manager);
  int _managers_vedis_exec(VedisManager *vedis_manager, char *zCmd, int nLen, const char *as_fmt, ...);
  int _managers_vedis_exec_result(VedisManager *vedis_manager, VedisValue **value);


  void vedis_test(VedisManager *vedis_manager);
  
#endif
