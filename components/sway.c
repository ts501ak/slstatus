/* See LICENSE file for copyright and license details. */
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <json-c/json.h>
#include <json-c/json_object.h>

#include "../util.h"

static int fd;

void sway_ipc_connect(void) {
  const char *env = getenv("SWAYSOCK");
  if (env == NULL) {
    die("Sway not running");
    return;
  }

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    die("Sway socket connection error");
    return;
  }

  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  memcpy(addr.sun_path, env, sizeof(addr.sun_path));
  if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
    die("Sway connection error");
    return;
  }
}

void sway_ipc_disconnect(void) {
  if (write(fd, "close-sway-ipc", 14) < 0)
    warn("Failed to write close-sway-ipc");

  close(fd);
}

const char *sway_keymap(void) {
  ssize_t total, res;
  char header[14], *payload;
  const char *layoutname, *ret;
  uint32_t *data32, payloadSize;
  json_object *inputs, *it, *layout;

  memcpy(header, "i3-ipc", 6);
  data32 = (uint32_t *)(header + 6);
  data32[0] = 0;
  data32[1] = 100;
  if (send(fd, header, 14, 0) < 0) {
    warn("Failed to send sway-ipc header");
    return NULL;
  }

  total = 0;
  memset(header, 0, 14);
  data32 = (uint32_t *)(header + 6);
  while (total < 14) {
    if ((res = recv(fd, (void *)(header + total), (14 - total), 0)) < 0) {
      warn("Failed to receive sway-ipc header");
      return NULL;
    }
    total += res;
  }
  payloadSize = data32[0];
  payload = (char *) malloc(payloadSize);
  total = 0;
  while (total < payloadSize) {
    if ((res = recv(fd, (void *)(payload), (payloadSize - total), 0)) < 0) {
      warn("Failed to receive sway-ipc payload");
      return NULL;
    }
    total += res;
  }

  ret = NULL;
  inputs = json_tokener_parse(payload);
  for (size_t i = 0; i < json_object_array_length(inputs); i++) {
    if ((it = json_object_array_get_idx(inputs, i))) {
      if (json_object_object_get_ex(it, "xkb_active_layout_name", &layout)) {
        if ((layoutname = json_object_get_string(layout))) {
          ret = bprintf("%s", layoutname);
          break;
        }
      }
    }
  }
  json_object_put(inputs);
  free(payload);
  if(!ret)
      warn("Failed to evaluate sway-inputs");

  return ret;
}
