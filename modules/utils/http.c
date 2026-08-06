// MODULES / UTILS / HTTP.C

#include "http.h" // modules/utils/http.h
#include "../globals.h" // modules/globals.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../_libs/http/civetweb.h"


int _utils_http_get_available_port(void) {
  int port = 8000;
  const int port_max = 65535;
  char port_str[6];

  while (port < port_max) {
    snprintf(port_str, sizeof(port_str), "%d", port);
    const char *options[] = { "listening_ports", port_str, NULL };

    struct mg_context *ctx = mg_start(NULL, NULL, options);
    
    if (ctx != NULL) { mg_stop(ctx); return port; }
    
    port++;
  }
  
  _globals_app_rc_log(ERR_MANAGERS_HTTP_PORT_UNAVAILABLE);
  return ERR_MANAGERS_HTTP_PORT_UNAVAILABLE; 
}
