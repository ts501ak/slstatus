/* See LICENSE file for copyright and license details. */
#include <string.h>
#include <unistd.h>
#include <pulse/context.h>
#include <pulse/mainloop.h>
#include <pulse/introspect.h>
#include <pulse/pulseaudio.h>

#include "../util.h"

static char state;
static pa_context *context;
static pa_mainloop_api  *mainloop_api;
static pa_mainloop *mainloop;

static void 
iterate(pa_operation *op) 
{
  while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
    pa_mainloop_iterate(mainloop, 1, NULL);
    usleep(100);
  }
}

static 
void context_notify_cb(pa_context *c, void * UNUSED(userdata)) 
{
  switch(pa_context_get_state(c)) {
  case PA_CONTEXT_UNCONNECTED:
  case PA_CONTEXT_CONNECTING:
  case PA_CONTEXT_AUTHORIZING:
  case PA_CONTEXT_SETTING_NAME:
  case PA_CONTEXT_TERMINATED:
    /* fallthrough */
    break;
  case PA_CONTEXT_READY:
    state = 0;
    break;
  case PA_CONTEXT_FAILED:
    state = 2;
    break;
  }
}

static void
server_info_cb_default_sink(pa_context * UNUSED(c), const pa_server_info*i,
                            void *userdata) 
{
  memcpy(userdata, i->default_sink_name, strlen(i->default_sink_name)+ 1);
}

static void
server_info_cb_default_source(pa_context * UNUSED(c), const pa_server_info*i, 
                              void *userdata) 
{
  memcpy(userdata, i->default_source_name, strlen(i->default_source_name) + 1);
}

static void
sink_info_volume_cb(pa_context * UNUSED(c), const pa_sink_info *i, int eol, 
                    void *userdata) 
{
  if (eol != 0)
    return;

  *((uint32_t*) userdata) = 
    pa_cvolume_avg(&i->volume) * 100. / PA_VOLUME_NORM + .5;
}

static void
source_info_volume_cb(pa_context * UNUSED(c), const pa_source_info *i, int eol,
                      void *userdata) 
{
  if (eol != 0)
    return;

  *((uint32_t*) userdata) = 
    pa_cvolume_avg(&i->volume) * 100. / PA_VOLUME_NORM + .5;
}

static void
sink_info_muted_cb(pa_context * UNUSED(c), const pa_sink_info *i, int eol,
                   void *userdata) 
{
  if (eol != 0)
    return;

  *((size_t *)userdata) = i->mute;
}

static void
source_info_muted_cb(pa_context * UNUSED(c), const pa_source_info *i, int eol,
                     void *userdata) 
{
  if (eol != 0)
    return;

  *((size_t*)userdata) = i->mute;
}

static inline void
get_default_sink_name(pa_operation *op, char * c) 
{
  op = pa_context_get_server_info(context, &server_info_cb_default_sink, c);
  iterate(op);
}

static inline void
get_default_source_name(pa_operation *op, char * c) 
{
  op = pa_context_get_server_info(context, &server_info_cb_default_source, c);
  iterate(op);
}

void 
pulse_connect(void) 
{
  state = 1;
  mainloop = pa_mainloop_new();
  mainloop_api = pa_mainloop_get_api(mainloop);
  context = pa_context_new(mainloop_api, "slstatus");
  pa_context_set_state_callback(context, &context_notify_cb, &state);
  if (pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
    die("Pulseaudio connection error");
    return;
  }

  while (state == 1) 
    pa_mainloop_iterate(mainloop, 1, NULL);

  if (state == 2) {
    die("Pulseaudio connection error");
    return; 
  }
}

void 
pulse_disconnect(void) 
{
  pa_context_set_state_callback(context, NULL, NULL);
  pa_context_disconnect(context);
  pa_context_unref(context);
  pa_mainloop_free(mainloop);
}

const char *
default_sink_volume(void) {
  uint32_t volume_perc;
  char default_sink_name[1024];
  pa_operation *uninitialized_var(op);
  get_default_sink_name(op, default_sink_name);
  op = pa_context_get_sink_info_by_name(context, default_sink_name, 
                                        &sink_info_volume_cb , &volume_perc);
  iterate(op);
  return bprintf("%d", volume_perc);
}

const char *
default_sink_mute(void) {
  size_t muted;
  char default_sink_name[1024];
  pa_operation *uninitialized_var(op);
  static char *map[] = { "\ufa7d", "\ufa80" };
  get_default_sink_name(op, default_sink_name);
  op = pa_context_get_sink_info_by_name(context, default_sink_name, 
                                        &sink_info_muted_cb, &muted);
  iterate(op);
  return bprintf("%s", map[muted]);
}

const char *
default_source_volume(void) {
  uint32_t volume_perc;
  char default_source_name[1024];
  pa_operation *uninitialized_var(op);
  get_default_source_name(op, default_source_name);
  op = pa_context_get_source_info_by_name(context, default_source_name, 
                                          &source_info_volume_cb, &volume_perc);
  iterate(op);
  return bprintf("%d", volume_perc);
}

const char *
default_source_mute(void) {
  size_t muted;
  char default_source_name[1024];
  pa_operation *uninitialized_var(op);
  static char *map[] = { "\uf86b", "\uf86c" };
  get_default_source_name(op, default_source_name);
  op = pa_context_get_source_info_by_name(context, default_source_name, 
                                          &source_info_muted_cb, &muted);
  iterate(op);
  return bprintf("%s", map[muted]);
}
