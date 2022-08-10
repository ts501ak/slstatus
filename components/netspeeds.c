/* See LICENSE file for copyright and license details. */
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "../util.h"

const char *netspeed_rx(const char *interface) {
  uintmax_t oldrxbytes;
  static uintmax_t rxbytes;
  extern const unsigned int interval;
  char path[PATH_MAX];

  oldrxbytes = rxbytes;

  if (esnprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes",
                interface) < 0) {
    return NULL;
  }
  if (pscanf(path, "%ju", &rxbytes) != 1) {
    return NULL;
  }
  if (oldrxbytes == 0) {
    return NULL;
  }

  return fmt_human((rxbytes - oldrxbytes) * 1000 / interval, 1024);
}

const char *netspeed_tx(const char *interface) {
  uintmax_t oldtxbytes;
  static uintmax_t txbytes;
  extern const unsigned int interval;
  char path[PATH_MAX];

  oldtxbytes = txbytes;

  if (esnprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes",
                interface) < 0) {
    return NULL;
  }
  if (pscanf(path, "%ju", &txbytes) != 1) {
    return NULL;
  }
  if (oldtxbytes == 0) {
    return NULL;
  }

  return fmt_human((txbytes - oldtxbytes) * 1000 / interval, 1024);
}
