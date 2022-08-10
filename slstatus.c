/* See LICENSE file for copyright and license details. */
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "util.h"
#include "slstatus.h"

struct arg {
  const uint16_t interval;
  const uint8_t signal;
  const char *(*func)();
  const char *fmt;
  const char *args;
};

static volatile sig_atomic_t done;

#include "config.h"

char buf[1024];
char buffers[LEN(args)][1024] = {0};

static int 
updatearg(const size_t i) {
  const char *res;
  if (!(res = args[i].func(args[i].args))) 
    res = unknown_str;

  return esnprintf(buffers[i], 1024, args[i].fmt, res);
}

static void 
signalhandler(const int signo) {
  size_t i;

  switch (signo) {
  case SIGINT:
    /* fallthrough */
  case SIGTERM:
    done = 1;
    break;
  case SIGUSR1:
    break;
  default:
    for (i = 0; i < LEN(args); i++) {
      if (signo - SIGRTMIN == args[i].signal) {
        updatearg(i);
        break;
      }
    }
  }
}

static void 
difftimespec(struct timespec *res, struct timespec *a,
                         struct timespec *b) {
  res->tv_sec = a->tv_sec - b->tv_sec - (a->tv_nsec < b->tv_nsec);
  res->tv_nsec = a->tv_nsec - b->tv_nsec + (a->tv_nsec < b->tv_nsec) * 1E9;
}

static void usage(void) { die("usage: %s [-1]", argv0); }

int main(int argc, char *argv[]) {
  size_t i;
  uint16_t counter;
  struct sigaction act;
  struct timespec start, current, diff, intspec, wait;

  ARGBEGIN {
  case '1':
    done = 1;
    break;
  default:
    usage();
  }
  ARGEND

  if (argc) 
    usage();

#ifdef USE_SWAY
  sway_ipc_connect();  
#endif

#ifdef USE_PULSE
  pulse_connect();
#endif

  memset(&act, 0, sizeof(act));
  act.sa_handler = signalhandler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);
  for (i = 0; i < LEN(args); i++) {
    updatearg(i);
    if (args[i].signal) 
      sigaction(SIGRTMIN + args[i].signal, &act, NULL);
  }
  act.sa_flags |= SA_RESTART;
  sigaction(SIGUSR1, &act, NULL);

  counter = 0;
  do {
    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) 
      die("clock_gettime:");

    for (i = 0; i < LEN(args); i++) {
      if (args[i].interval && !(counter % args[i].interval))
        if (updatearg(i) < 0)
          break;

      fputs(buffers[i], stdout);
      if (ferror(stdout))
        die("puts:");
    }

    fflush(stdout);

    if (!done) {
      counter++;
      if (clock_gettime(CLOCK_MONOTONIC, &current) < 0)
        die("clock_gettime:");
      difftimespec(&diff, &current, &start);

      intspec.tv_sec = interval / 1000;
      intspec.tv_nsec = (interval % 1000) * 1E6;
      difftimespec(&wait, &intspec, &diff);

      if (wait.tv_sec >= 0) 
        if (nanosleep(&wait, NULL) < 0 && errno != EINTR) 
          die("nanosleep:");
    }
  } while (!done);

#ifdef USE_SWAY
  sway_ipc_disconnect();
#endif

#ifdef USE_PULSE
  pulse_disconnect();
#endif

  return 0;
}
