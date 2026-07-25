#include "log.h"
#include "send.h"
#include "nxjson.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define PROTOCOL_END_MARKER     "\nEND"
#define PROTOCOL_END_MARKER_LEN 4
#define PROTOCOL_BUFFER_SIZE    4096

static void* Mem_Realloc(void* ptr, size_t size) {
  void* new_ptr = realloc(ptr, size);

  if (! new_ptr)
    free(ptr);
  
  return new_ptr;
}

static int ReceiveJson(int socket, char** buf, const nx_json** out) {
  char buffer[PROTOCOL_BUFFER_SIZE];
  ssize_t nread;
  const nx_json* json = NULL;

  char* msg = NULL;
  size_t msg_size = 0;

  while ((nread = read(socket, buffer, PROTOCOL_BUFFER_SIZE)) > 0) {
    msg = Mem_Realloc(msg, msg_size + (size_t) nread + 1);
    if (! msg)
      return -1;

    memcpy(msg + msg_size, buffer, (size_t) nread);
    msg_size += (size_t) nread;
    msg[msg_size] = '\0';

    char *end_marker_pos = strstr(msg, PROTOCOL_END_MARKER);
    if (end_marker_pos != NULL) {
      *end_marker_pos = '\0';
      break;
    }
  }

  if (! msg) {
    WARN("Received empty response");
    return -1;
  }

  json = nx_json_parse_utf8(msg);
  if (! json) {
    free(msg);
    WARN("Received invalid JSON");
    return -1;
  }

  *buf = msg;
  *out = json;
  return 0;
}

static int CreateSocket(const char* socket_path) {
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) {
    WARN("socket() failed");
    return -1;
  }

  struct sockaddr_un serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  snprintf(serv_addr.sun_path, sizeof(serv_addr.sun_path), socket_path);

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    WARN("%s: %s", "connect()", socket_path);
    close(sock);
    return -1;
  }

  return sock;
}

static int CheckResponse(const char* trace, const nx_json* response) {
  if (response->type != NX_JSON_OBJECT) {
    WARN("%s: Not a JSON object", trace);
    return -1;
  }

  const nx_json* err = nx_json_get(response, "Error");
  if (err) {
    if (err->type == NX_JSON_STRING)
      WARN("%s: %s", err->val.text, trace);
    else
      WARN("%s: 'Error' is not a string", trace);

    return -1;
  }

  return 0;
}

static int ServiceStatus(const char* socket_path, const nx_json** out, char** buf) {
  const char* const in = "{\"Command\":\"status\"}\nEND";
  const int sock = CreateSocket(socket_path);

  *buf = NULL;
  *out = NULL;

  if (sock < 0)
    return -1;

  if (! Send(sock, in, strlen(in))) {
    WARN("send(): %s", strerror(errno));
    goto error;
  }

  if (ReceiveJson(sock, buf, out) != 0)
    goto error;

  if (CheckResponse("status", *out) != 0)
    goto error;

  close(sock);
  return 0;

error:
  close(sock);
  free(*buf);
  nx_json_free(*out);
  return -1;
}

static int SetFanSpeed(const char* socket_path, int fan, float speed) {
  int ret = -1;
  char in[128];
  char* buf = NULL;
  const nx_json* response= NULL;
  const int sock = CreateSocket(socket_path);

  if (sock < 0)
    return -1;

  if (speed == -1.0f) {
    snprintf(in, sizeof(in),
      "{\"Command\":\"set-fan-speed\",\"Fan\":%d,\"Speed\":\"auto\"}\nEND",
      fan);
  }
  else {
    snprintf(in, sizeof(in),
      "{\"Command\":\"set-fan-speed\",\"Fan\":%d,\"Speed\":%f}\nEND",
      fan, speed);
  }

  if (! Send(sock, in, strlen(in))) {
    WARN("send(): %s", strerror(errno));
    goto end;
  }

  if (ReceiveJson(sock, &buf, &response) != 0)
    goto end;

  if (CheckResponse("set-fan-speed", response) != 0)
    goto end;

  ret = 0;

end:
  close(sock);
  free(buf);
  nx_json_free(response);
  return ret;
}
