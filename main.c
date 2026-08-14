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
  //added:10446
    //#include <string.h>
  //added:10887
    //memset(pBlock, 0, nByte);

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


#define VEDIS_MANAGER_STORE_COUNT_MAX 10


typedef struct {
  const char *command;
  const char *description;
} Command;
Command commands[] = {
  { "/close /stop /exit /quit",     "Clean exit"},
  { "/restart",                     "Restart" },
  { "/test_vedis",                  "Vedis values test" },
  #if defined(PLOUCKY_ENABLE_VEDIS_CMD)
  { "/vedis",                       "Use vedis from cli\n       list\n       SET my_int 42\n       GET my_int\n       dbtest:SET my_str test\n       dbtest:GET my_str" },
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
int main(int argc, char* argv[]) { // MAIN
  signal(SIGINT, _main_handle_signal);
  signal(SIGTERM, _main_handle_signal);
  int rc;

  while(restart_requested) {
    stop_requested = 0; restart_requested = 0;
    
    
    // VEDIS
    VedisManagers *vedis_managers = calloc(1, sizeof(VedisManagers));
    if (!vedis_managers) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_CALLOC); return ERR_MANAGERS_VEDIS_CALLOC; }
    vedis_managers->managers = calloc(VEDIS_MANAGER_STORE_COUNT_MAX, sizeof(VedisManager)); // 10 VedisManager max !
    if (!vedis_managers->managers) { _globals_app_rc_log(ERR_MANAGERS_VEDIS_CALLOC); return ERR_MANAGERS_VEDIS_CALLOC; } // change/add rc
    
    
    // MONGOOSE
    HttpManager *http_manager = calloc(1, sizeof(HttpManager));
    if (!http_manager) { _globals_app_rc_log(ERR_MANAGERS_HTTP_CALLOC); return ERR_MANAGERS_HTTP_CALLOC; }
    
    int port = _utils_http_get_available_port();
    if (port == ERR_MANAGERS_HTTP_PORT_UNAVAILABLE) {
      return ERR_MANAGERS_HTTP_PORT_UNAVAILABLE;
    }
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    
    char db_path[256]; // GLOBAL PATH
    snprintf(db_path, sizeof(db_path), "$HOME/.ploucky/%s", port_str);
    // ADD STORES HERE // adjust VEDIS_MANAGER_STORE_COUNT_MAX with the number of stores
    _managers_vedis_store_add(vedis_managers, NULL, "memory");
    char db_path_test[256]; // PATH BY FILE
    snprintf(db_path_test, sizeof(db_path_test), "%s/db_test", db_path);
    _managers_vedis_store_add(vedis_managers, db_path_test, "dbtest");
    
    
    
    rc = _managers_http_init(http_manager, vedis_managers, port_str);
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
              if (strncmp(linenoise_line, "/close", strlen("/close")) == 0 || strncmp(linenoise_line, "/stop", strlen("/stop")) == 0 || strncmp(linenoise_line, "/exit", strlen("/exit")) == 0 || strncmp(linenoise_line, "/quit", strlen("/quit")) == 0) { break; }
              if (strncmp(linenoise_line, "/restart", strlen("/restart")) == 0) { restart_requested = 1; break; }
              linenoiseEditStop(&linenoise_state);
              memset(linenoise_buf, 0, sizeof(linenoise_buf));
              memset(&linenoise_state, 0, sizeof(linenoise_state));
              // others cmd
              if (linenoise_line) { if (strncmp(linenoise_line, "/help", strlen("/help")) == 0) { _main_print_cmd_help(); } }
              if (linenoise_line) { if (strncmp(linenoise_line, "/test_vedis", strlen("/test_vedis")) == 0) { vedis_test(vedis_managers); } }
              #if defined(PLOUCKY_ENABLE_VEDIS_CMD)
                if (linenoise_line) { if (strncmp(linenoise_line, "/vedis ", strlen("/vedis ")) == 0) { _managers_vedis_exec_cmdcli(vedis_managers, linenoise_line); } }
              #endif
              #if defined(MG_EXPERIMENTAL_INTERFACES) && defined(USE_SERVER_STATS)
                if (linenoise_line) { if (strncmp(linenoise_line, "/connections", strlen("/connections")) == 0) { print_http_connections(http_manager->ctx); } }
              #endif
              #if defined(USE_SERVER_STATS)
                if (linenoise_line) { if (strncmp(linenoise_line, "/stats", strlen("/stats")) == 0) { print_http_stats(http_manager->ctx); } }
              #endif
              _utils_printf(NULL, "\n");
            } else {
              memset(linenoise_buf, 0, sizeof(linenoise_buf));
              memset(&linenoise_state, 0, sizeof(linenoise_state));
              if (linenoise_line[0] != '\0') { _utils_printf(NULL, "\n- Command not found\n\n"); }
            }
            
            free(linenoise_line); linenoise_line = NULL; 
          } else {
            break; // SIG
          }
          
          is_command_exist = 0;
          linenoiseEditStart(&linenoise_state, -1, -1, linenoise_buf, sizeof(linenoise_buf), ">>> ");
        }
      }
    }
    free(linenoise_line);
    linenoise_line = NULL; 
    linenoiseEditStop(&linenoise_state);
    memset(linenoise_buf, 0, sizeof(linenoise_buf));
    memset(&linenoise_state, 0, sizeof(linenoise_state));
    
    rc = _managers_http_close(http_manager);
    rc = _managers_vedis_close(vedis_managers);
    
    free(http_manager);
    free(vedis_managers);
    mg_exit_library(); // mongoose
    vedis_lib_shutdown(); // vedis
  }
  return rc;
}
