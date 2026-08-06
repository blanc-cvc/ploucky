// MODULES / MANAGERS / HTTP.H

#ifndef MANAGERS_HTTP_H
  #define MANAGERS_HTTP_H

  #include "../_libs/http/civetweb.h"
  #include "vedis.h" // modules/managers/vedis.h
  
  typedef struct {
      struct mg_context *ctx;
      VedisManager *vedis_manager;
  } HttpManager;
  
  int _managers_http_init(HttpManager *http_manager, VedisManager *vedis_manager, const char *port);
  int _managers_http_close(HttpManager *http_manager);
  
  #if defined(MG_EXPERIMENTAL_INTERFACES) && defined(USE_SERVER_STATS)
    void print_http_connections(struct mg_context *ctx);
  #endif
  #if defined(USE_SERVER_STATS)
    void print_http_stats(struct mg_context *ctx);
  #endif
  
#endif
