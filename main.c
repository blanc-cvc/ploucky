//thanks to search.brave.com

//#include "modules/_libs/http/civetweb.h"
//added:2597
  //#if defined(NO_THREAD_NAME) && defined(__linux__)
  //#include <sys/sendfile.h>
  //#endif

//modules/_libs/vedis/vedis.c
//replaced:9018
  //pMap = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE|MAP_FILE, fd, 0);
  //pMap = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

#include "modules/globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include "modules/utils/print.h"
#include "modules/utils/vedis.h"
#include "modules/utils/http.h"
#include "modules/managers/vedis.h"
#include "modules/managers/http.h"

#include "modules/_libs/linenoise/linenoise.h"


struct linenoiseState linenoise_state;
volatile sig_atomic_t stop_requested = 0;
sig_atomic_t restart_requested = 1;
static void handle_signal(int sig) { (void)sig; restart_requested = 0; stop_requested = 1; }
int main(int argc, char* argv[]) {
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);
  int rc;
    
  while(restart_requested) {
    stop_requested = 0; restart_requested = 0;
    
    
    //
    VedisManager *vedis_manager_mem = calloc(1, sizeof(VedisManager));
    if (!vedis_manager_mem) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_CALLOC); return ERR_MANAGERS_VEDIS_CALLOC; }
    
    rc = _managers_vedis_init(vedis_manager_mem, NULL);
    if (rc != OK_MANAGERS_VEDIS_INIT) { return rc; }
    
    
    //
    HttpManager *http_manager = calloc(1, sizeof(HttpManager));
    if (!http_manager) { _globals_app_rc_log(ERR_MANAGERS_HTTP_CALLOC); return ERR_MANAGERS_HTTP_CALLOC; }
    
    int port = _utils_http_get_available_port();
    if (port == ERR_MANAGERS_HTTP_PORT_UNAVAILABLE) {
      return ERR_MANAGERS_HTTP_PORT_UNAVAILABLE;
    }
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    rc = _managers_http_init(http_manager, vedis_manager_mem, port_str);
    if (rc != OK_MANAGERS_HTTP_INIT) { return rc; }
    
    
    //
    char linenoise_buf[1024];
    char *linenoise_line = NULL;
    _utils_printf(NULL, "\n");
    linenoiseEditStart(&linenoise_state, -1, -1, linenoise_buf, sizeof(linenoise_buf), ">>> ");
    fd_set fds;
    while (!stop_requested) {
      FD_ZERO(&fds);
      FD_SET(STDIN_FILENO, &fds);
      struct timeval timeout = { 2, 0 };
      int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout);
      if (ret == -1) {
        break;
      } else if (ret == 0) {
        continue; // at timeout, condition re-evaluation
      } else {
        // LINENOISE
        linenoise_line = linenoiseEditFeed(&linenoise_state);
        if (linenoise_line != linenoiseEditMore) {
            linenoiseEditStop(&linenoise_state);
            if (linenoise_line) {
                linenoiseHistoryAdd(linenoise_line);
                // break cmd
                if (strcmp(linenoise_line, "/close") == 0 || strcmp(linenoise_line, "/stop") == 0 || strcmp(linenoise_line, "/exit") == 0 || strcmp(linenoise_line, "/quit") == 0) { free(linenoise_line); break; }
                if (strcmp(linenoise_line, "/restart") == 0) { free(linenoise_line); restart_requested = 1; break; }
                linenoiseEditStop(&linenoise_state);
                memset(linenoise_buf, 0, sizeof(linenoise_buf));
                memset(&linenoise_state, 0, sizeof(linenoise_state));
                // others cmd
                if (linenoise_line) { if (strcmp(linenoise_line, "/test_vedis") == 0) { free(linenoise_line); linenoise_line = NULL; vedis_test(vedis_manager_mem); } }
                #if defined(MG_EXPERIMENTAL_INTERFACES) && defined(USE_SERVER_STATS)
                  if (linenoise_line) { if (strcmp(linenoise_line, "/connections") == 0) { free(linenoise_line); linenoise_line = NULL; print_http_connections(http_manager->ctx); } }
                #endif
                #if defined(USE_SERVER_STATS)
                  if (linenoise_line) { if (strcmp(linenoise_line, "/stats") == 0) { free(linenoise_line); linenoise_line = NULL; print_http_stats(http_manager->ctx); } }
                #endif
            } else {
                break; // SIG
            }
            if (linenoise_buf[0] != '\0') { _utils_printf(NULL, "\n"); }
            linenoiseEditStart(&linenoise_state, -1, -1, linenoise_buf, sizeof(linenoise_buf), ">>> ");
        }
        // WITHOUT LINENOISE
        /*
        char buffer[100];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) { break; }
        // remove \n at the end
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') { buffer[len - 1] = '\0'; }
        // check if is exit or quit
        if (strcmp(buffer, "/stop") == 0 || strcmp(buffer, "/exit") == 0 || strcmp(buffer, "/quit") == 0) { break; }
        if (strcmp(buffer, "/restart") == 0) { restart_requested = 1; break; }
        if (strlen(buffer) == 0) continue;
        
        _utils_printf(NULL, "INPUT: %s\n", buffer);
        */
      }
    }
    linenoise_line = NULL; 
    linenoiseEditStop(&linenoise_state);
    memset(linenoise_buf, 0, sizeof(linenoise_buf));
    memset(&linenoise_state, 0, sizeof(linenoise_state));
    
    rc = _managers_http_close(http_manager);
    rc = _managers_vedis_close(vedis_manager_mem);
    
    free(http_manager);
    free(vedis_manager_mem);
    mg_exit_library(); // http civetweb
  }
  return rc;
}
