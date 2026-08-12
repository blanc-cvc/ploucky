// MODULES / MANAGERS / VEDIS.H

#ifndef MANAGERS_VEDIS_H
  #define MANAGERS_VEDIS_H

  #include "../_libs/vedis/vedis.h"
  #include <pthread.h>
  #include <stdarg.h>
  #include "../utils/vedis.h"

  typedef struct {
    const char *name;
    const char *path;
    vedis *pStore;
    pthread_mutex_t lock;
  } VedisManager;
  
  typedef struct {
    VedisManager *managers;
    size_t count;
  } VedisManagers;

  void _managers_vedis_handle_rc(int rc);
  VedisManager *_managers_vedis_manager_get(VedisManagers *vedis_managers, const char *name);
  int _managers_vedis_store_add(VedisManagers *vedis_managers, const char *storage_path, const char *name);
  int _managers_vedis_close(VedisManagers *vedis_managers);
  int _managers_vedis_exec(VedisManager *vedis_manager, char *zCmd, int nLen, const char *as_fmt, ...);
  int _managers_vedis_exec_result(VedisManager *vedis_manager, VedisValue **value);
  #if defined(PLOUCKY_ENABLE_VEDIS_CMD)
    int _managers_vedis_exec_cmdcli(VedisManagers *vedis_managers, const char *cmd);
  #endif

  void vedis_test(VedisManagers *vedis_managers);
  
#endif
