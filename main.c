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
#include <strings.h>

#include "modules/utils/print.h"
#include "modules/utils/vedis.h"
#include "modules/utils/http.h"
#include "modules/managers/vedis.h"
#include "modules/managers/http.h"

#include "modules/_libs/linenoise/linenoise.h"


typedef struct {
  const char *command;
  const char *description;
} Command;


Command commands[] = {
  { "/close /stop /exit /quit",     "Clean exit"},
  { "/restart",                     "Restart" },
  { "/test_vedis",                  "Vedis values test" },
  #if defined(PLOUCKY_ENABLE_VEDIS_CMD)
  { "/vedis",                       "Use vedis from cli: /vedis SET my_int 42" },
  #endif
  #if defined(MG_EXPERIMENTAL_INTERFACES) && defined(USE_SERVER_STATS)
  { "/connections",                 "Print http connections" },
  #endif
  #if defined(USE_SERVER_STATS)
  { "/stats",                       "Print http stats" },
  #endif
  { "/help",                        "Display this help" },
  { NULL,             NULL }
};

void _main_print_cmd_help(void) {
  for (int i = 0; commands[i].command != NULL; i++) {
    if (strcmp(commands[i].command, "/help") == 0) continue; 
      
    _utils_printf(NULL, "%-25s : %s\n", commands[i].command, commands[i].description);
  }
}

struct linenoiseState linenoise_state;
void linenoise_completion(const char *buf, linenoiseCompletions *lc) {
  for (int i = 0; commands[i].command != NULL; i++) {
    if (strncmp(commands[i].command, "/close", 6) == 0) {
      char *str = strdup(commands[i].command);
      char *token;
      token = strtok(str, " ");
      while (token != NULL) {
        if (strncmp(token, buf, strlen(buf)) == 0) {
          linenoiseAddCompletion(lc, token);
        }
        token = strtok(NULL, " ");
      }
      free(str);
    } else {
      if (strncmp(commands[i].command, buf, strlen(buf)) == 0) {
        linenoiseAddCompletion(lc, commands[i].command);
      }
    }
  }
}
volatile sig_atomic_t stop_requested = 0;
sig_atomic_t restart_requested = 1;
static void _main_handle_signal(int sig) { (void)sig; restart_requested = 0; stop_requested = 1; }
int main(int argc, char* argv[]) {
  signal(SIGINT, _main_handle_signal);
  signal(SIGTERM, _main_handle_signal);
  int rc;

  while(restart_requested) {
    stop_requested = 0; restart_requested = 0;
    
    
    // VEDIS
    VedisManager *vedis_manager_mem = calloc(1, sizeof(VedisManager));
    if (!vedis_manager_mem) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_CALLOC); return ERR_MANAGERS_VEDIS_CALLOC; }
    
    rc = _managers_vedis_init(vedis_manager_mem, NULL);
    if (rc != OK_MANAGERS_VEDIS_INIT) { return rc; }
    
    
    // MONGOOSE
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
    
    
    _utils_printf(NULL, "\n- Display commands with /help\n");
    // LINENOISE
    char linenoise_buf[1024];
    char *linenoise_line = NULL;
    _utils_printf(NULL, "\n");
    linenoiseEditStart(&linenoise_state, -1, -1, linenoise_buf, sizeof(linenoise_buf), ">>> ");
    linenoiseSetCompletionCallback(linenoise_completion);
    fd_set fds;
    while (!stop_requested) {
      int is_command_exist = 0;
      FD_ZERO(&fds);
      FD_SET(STDIN_FILENO, &fds);
      struct timeval timeout = { 2, 0 };
      int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout);
      if (ret == -1) {
        break;
      } else if (ret == 0) {
        continue; // at timeout, condition re-evaluation
      } else {
        linenoise_line = linenoiseEditFeed(&linenoise_state);
        if (linenoise_line != linenoiseEditMore) {
          linenoiseEditStop(&linenoise_state);
          
            if (linenoise_line) {
              for (int i = 0; commands[i].command != NULL; i++) {    
                if (strncmp(commands[i].command, "/close", 6) == 0) {
                  char *str = strdup(commands[i].command);
                  char *token;
                  token = strtok(str, " ");
                  while (token != NULL) {
                    if (strncmp(linenoise_line, token, strlen(token)) == 0) {
                      is_command_exist = 1;
                    }
                    token = strtok(NULL, " ");
                  }
                  free(str);
                }
                if (strncmp(linenoise_line, commands[i].command, strlen(commands[i].command)) == 0) { is_command_exist = 1; }
              }
              if (is_command_exist) {
                linenoiseHistoryAdd(linenoise_line);
                // break cmd
                if (strncmp(linenoise_line, "/close", strlen("/close")) == 0 || strncmp(linenoise_line, "/stop", strlen("/stop")) == 0 || strncmp(linenoise_line, "/exit", strlen("/exit")) == 0 || strncmp(linenoise_line, "/quit", strlen("/quit")) == 0) { free(linenoise_line); break; }
                if (strncmp(linenoise_line, "/restart", strlen("/restart")) == 0) { free(linenoise_line); restart_requested = 1; break; }
                linenoiseEditStop(&linenoise_state);
                memset(linenoise_buf, 0, sizeof(linenoise_buf));
                memset(&linenoise_state, 0, sizeof(linenoise_state));
                // others cmd
                if (linenoise_line) { if (strncmp(linenoise_line, "/help", strlen("/help")) == 0) { free(linenoise_line); linenoise_line = NULL; _main_print_cmd_help(); } }
                if (linenoise_line) { if (strncmp(linenoise_line, "/test_vedis", strlen("/test_vedis")) == 0) { free(linenoise_line); linenoise_line = NULL; vedis_test(vedis_manager_mem); } }
                #if defined(PLOUCKY_ENABLE_VEDIS_CMD)
                  if (linenoise_line) { if (strncmp(linenoise_line, "/vedis ", strlen("/vedis ")) == 0) { _managers_vedis_exec_cmdcli(vedis_manager_mem, linenoise_line); free(linenoise_line); linenoise_line = NULL; } }
                #endif
                #if defined(MG_EXPERIMENTAL_INTERFACES) && defined(USE_SERVER_STATS)
                  if (linenoise_line) { if (strncmp(linenoise_line, "/connections", strlen("/connections")) == 0) { free(linenoise_line); linenoise_line = NULL; print_http_connections(http_manager->ctx); } }
                #endif
                #if defined(USE_SERVER_STATS)
                  if (linenoise_line) { if (strncmp(linenoise_line, "/stats", strlen("/stats")) == 0) { free(linenoise_line); linenoise_line = NULL; print_http_stats(http_manager->ctx); } }
                #endif
              } else {
                memset(linenoise_buf, 0, sizeof(linenoise_buf));
                memset(&linenoise_state, 0, sizeof(linenoise_state));
                if (linenoise_line[0] != '\0') { _utils_printf(NULL, "\n- Command not found\n"); }
              }
            } else {
              break; // SIG
            }
          
          if (!linenoise_line || linenoise_line[0] != '\0') { _utils_printf(NULL, "\n"); }
          is_command_exist = 0;
          linenoiseEditStart(&linenoise_state, -1, -1, linenoise_buf, sizeof(linenoise_buf), ">>> ");
        }
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
    mg_exit_library(); // mongoose
    vedis_lib_shutdown(); // vedis
  }
  return rc;
}
