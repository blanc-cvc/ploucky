// MODULES / MANAGERS / HTTP.C

#include "http.h" // modules/managers/http.h
#include "../globals.h" // modules/globals.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vedis.h" // modules/managers/vedis.h
#include "../utils/vedis.h"
#include "../utils/print.h"
#include "../utils/int.h"
#include "../_libs/cjson/cJSON.h"
#include "../_generated/web.h"


// PRIVATE FUNCTIONS
void print_request_details(struct mg_connection *conn) {
  const struct mg_request_info *req_info = mg_get_request_info(conn);    
  if (!req_info) { _utils_printf("err", "Error: mg_request_info NULL pointer.\n"); return; }

  _utils_printf(NULL, "\n----- Request details -----\n");
  _utils_printf(NULL, "[%s %s] %s:%d -> :%d%s\n", req_info->request_method, req_info->http_version, req_info->remote_addr, req_info->remote_port, req_info->server_port, req_info->local_uri);
  if (req_info->query_string != NULL) { _utils_printf(NULL, "  Query String  : %s\n", req_info->query_string); }
  _utils_printf(NULL, "Headers (%d)  :\n", req_info->num_headers);
  for (int i = 0; i < req_info->num_headers; i++) {
    _utils_printf(NULL, "  [%s]: %s\n", req_info->http_headers[i].name, req_info->http_headers[i].value);
  }
  _utils_printf(NULL, "---------------------------\n");
}

int send_http_response(struct mg_connection *conn, int code, const char *content_type, const char *content, long content_length) {
  const char *reason = code == 200 ? "OK" : code == 206 ? "Partial Content" : "Error";
  
  int is_chunked = (content_length < 0);
  content_length = is_chunked ? strlen(content) : (content_length == 0 && content) ? strlen(content) : content_length; // if len == 0 && content, compute size for not chunked
  int is_10 = is_chunked ? 0 : content_type == "text/plain" ? 1 : 0;

  char len_buf[32];
  const char *header_chunk_or_len = is_chunked
      ? "Transfer-Encoding: chunked\r\n"
      : (sprintf(len_buf, "Content-Length: %ld\r\n", content_length), len_buf);

  // 206 Partial Content: "Content-Type: video/mp4\r\n" "Content-Range: bytes %llu-%llu/%llu\r\n", start, end, file_size); "Content-Length: %llu\r\n", content_length); "Accept-Ranges: bytes\r\n");
  mg_printf(conn,
    "HTTP/%s %d %s\r\n"
    "Content-Type: %s; charset=utf-8\r\n"
    "%s"
    "Connection: %s\r\n"
    "Cache-Control: no-cache, no-store, must-revalidate\r\n"
    "Pragma: no-cache\r\n"
    "Expires: 0\r\n"
    "\r\n",
    is_10 ? "1.0" : "1.1", code, reason, content_type, header_chunk_or_len, is_10 ? "close" : "keep-alive"
  );

  if (!is_chunked) {
    mg_write(conn, content, (size_t)content_length);
  } else {
    _utils_printf(NULL, "Sending as chunks\n");
    size_t chunk_size = 1024;
    size_t offset = 0;
    while (offset < content_length) {
      size_t remaining = content_length - offset;
      size_t current_len = (remaining >= chunk_size) ? chunk_size : remaining;
      if (current_len < remaining) {
        while (current_len > 0) {
          unsigned char last_byte = (unsigned char)content[offset + current_len - 1];
          if ((last_byte & 0xC0) == 0x80) {
            current_len--;
          } else {
            break;
          }
        }
        if (current_len == 0) { _utils_printf("err", "\n\nStrange: Chunk current_len == 0\n\n"); }
      }
      int bytes_sent = mg_send_chunk(conn, content + offset, current_len); // start: content at offset
      //_utils_printf(NULL, "Chunk sent bytes: %d\n", bytes_sent);
      //_utils_printf(NULL, "Chunk sent char len: %d\n", current_len);
      //_utils_printf(NULL, "Chunk offset: %d\n\n", offset);
      if (bytes_sent <= 0) { return -1; }
      offset += current_len;
    }
    mg_send_chunk(conn, "", 0); 
  }
  
  return 0;
}

static int request_handler(struct mg_connection *conn, void *cb_data) {
    if (stop_requested || restart_requested) { _globals_app_rc_log(OK_MANAGERS_RESTART_STOP_RETRY); return OK_MANAGERS_RESTART_STOP_RETRY; }
    HttpManager *http_manager = (HttpManager *)cb_data;
        
    if (!http_manager || !http_manager->vedis_managers) {
      send_http_response(conn, _utils_int_get_random(400, 500), "text/plain", "Error", 5);
      _globals_app_rc_log(ERR_MANAGERS_HTTP_REQUEST_VEDIS_POINTER); return ERR_MANAGERS_HTTP_REQUEST_VEDIS_POINTER;
    }
    
    print_request_details(conn);
    
    const struct mg_request_info *req_info = mg_get_request_info(conn);
    
    if (strcmp(req_info->request_method, "GET") != 0) {
      _globals_app_rc_log(ERR_MANAGERS_HTTP_REQUEST_METHOD_NOT_HANDLED); return ERR_MANAGERS_HTTP_REQUEST_METHOD_NOT_HANDLED;
    }
    
    
    if (strcmp(req_info->local_uri, "/") == 0) { // WEB_PAGE_HTML
      send_http_response(conn, 200, "text/html", WEB_PAGE_HTML, -1);
    } else if (strcmp(req_info->local_uri, "/vedis") == 0) {  // GET CMD_LIST from VEDIS
      int rc;
      rc = _managers_vedis_exec(_managers_vedis_manager_get(http_manager->vedis_managers, "memory"), "CMD_LIST", -1, NULL);
      if (rc != VEDIS_OK) {
        send_http_response(conn, _utils_int_get_random(400, 500), "text/plain", "Error", 5);
        _utils_vedis_rc_log(rc); return rc;
      }
      
      VedisValue *value;
      rc = _managers_vedis_exec_result(_managers_vedis_manager_get(http_manager->vedis_managers, "memory"), &value);
      if (rc != VEDIS_OK) {
        send_http_response(conn, _utils_int_get_random(400, 500), "text/plain", "Error", 5);
        _utils_vedis_rc_log(rc); return rc;
      }
      
      if (_utils_vedis_parse_value_isok(value)) {
        //_utils_vedis_print_value(value);
        send_http_response(conn, 200, "text/plain", value->_string_ptr, strlen(value->_string_ptr));
        _utils_vedis_value_destroy(value);
      } else {
        send_http_response(conn, _utils_int_get_random(400, 500), "text/plain", "Error", 5);
        _globals_app_rc_log(ERR_MANAGERS_HTTP_REQUEST_VEDIS_PARSE_VALUE); return ERR_MANAGERS_HTTP_REQUEST_VEDIS_PARSE_VALUE;
      }
    } else {
      mg_close_connection(conn);
      //send_http_response(conn, _utils_int_get_random(400, 500), "text/plain", "Error", 5);
      _globals_app_rc_log(ERR_MANAGERS_HTTP_REQUEST_URI_NOT_HANDLED); return ERR_MANAGERS_HTTP_REQUEST_URI_NOT_HANDLED;
    }
        
    return 0;
}
// END OF PRIVATE FUNCTIONS


// PUBLIC FUNCTIONS
int _managers_http_init(HttpManager *http_manager, VedisManagers *vedis_managers, const char *port) {
  if (stop_requested || restart_requested) { _globals_app_rc_log(OK_MANAGERS_RESTART_STOP_RETRY); return OK_MANAGERS_RESTART_STOP_RETRY; }
  if (!http_manager || !vedis_managers) { _globals_app_rc_log(ERR_MANAGERS_HTTP_INIT_POINTER); return ERR_MANAGERS_HTTP_INIT_POINTER; } // || or port is not between valid range
  // as cbdata: http_manager containing vedis_manager
  // cbdata: the callback data to give to the handler when it is called.
  
  if (http_manager->vedis_managers) { return -1; } // _managers_http_init already done
  http_manager->vedis_managers = vedis_managers;

  const char *options[] = { "listening_ports", port, NULL };

  //(const struct mg_callbacks *callbacks, void *user_data, const char **configuration_options);
  http_manager->ctx = mg_start(NULL, http_manager, options);
  /* mg_callbacks : C'est le moteur. 
   * Vous l'utilisez pour la configuration globale, les logs, et les événements bas niveau (connexion/déconnexion, erreurs système).
   * Il n'est pas lié à une URL spécifique.
   */
  if (http_manager->ctx == NULL) {
    _globals_app_rc_log(ERR_MANAGERS_HTTP_INIT_CONTEXT);
    return ERR_MANAGERS_HTTP_INIT_CONTEXT;
  }
  
  mg_set_request_handler(http_manager->ctx, "/", request_handler, http_manager);
  
  _globals_app_rc_log(OK_MANAGERS_HTTP_INIT);
  _utils_printf(NULL, "\nHTTP available at port: %s.\n", port);
  return OK_MANAGERS_HTTP_INIT;
}


int _managers_http_close(HttpManager *http_manager) {
  if (http_manager && http_manager->ctx) {
    mg_stop(http_manager->ctx);
    http_manager->ctx = NULL;
    
    _globals_app_rc_log(OK_MANAGERS_HTTP_CLOSE);
    return OK_MANAGERS_HTTP_CLOSE;
  }
  _globals_app_rc_log(ERR_MANAGERS_HTTP_CLOSE);
  return ERR_MANAGERS_HTTP_CLOSE;
}


#if defined(MG_EXPERIMENTAL_INTERFACES) && defined(USE_SERVER_STATS)
  void print_http_connections(struct mg_context *ctx) {
    char buffer[256];
    int idx = 0;
    int idx_used = 0;
    
    _utils_printf(NULL, "------ Connections -------\n");
    while (mg_get_connection_info(ctx, idx, buffer, sizeof(buffer)) > 0) {
      if (strstr(buffer, "\"state\" : \"not used\"") == NULL) {
        cJSON *json_buffer_parsed = cJSON_Parse(buffer);
        char *json_print_buffer_parsed = cJSON_Print(json_buffer_parsed);
        _utils_printf(NULL, "Slot [%d] connection [%d]: %s\n", idx, idx_used, json_print_buffer_parsed);
        cJSON_Delete(json_buffer_parsed);
        free(json_print_buffer_parsed);
        idx_used++;
      }
      idx++;
    }
    _utils_printf(NULL, "Total: %d connections/%d\n", idx_used, idx);
    _utils_printf(NULL, "--------------------------\n");
  }
#endif
#if defined(USE_SERVER_STATS)
  void print_http_stats(struct mg_context *ctx) {
    char buffer[4096];
    int len;

    len = mg_get_context_info(ctx, buffer, sizeof(buffer));
    
    if (len <= sizeof(buffer)) {
      _utils_printf(NULL, "--------- Stats ----------\n");
      cJSON *json_buffer_parsed = cJSON_Parse(buffer);
      char *json_print_buffer_parsed = cJSON_Print(json_buffer_parsed);
      _utils_printf(NULL, "%s\n", json_print_buffer_parsed);
      cJSON_Delete(json_buffer_parsed);
      free(json_print_buffer_parsed);
      _utils_printf(NULL, "--------------------------\n");
    } else {
      _utils_printf(NULL, "Stats error: buffer too small.\n");
    }
  }
#endif
// END OF PUBLIC FUNCTIONS
