/* See LICENSE file for copyright and license details. */

const unsigned int interval = 1000;
static const char unknown_str[] = "n/a";

static const struct arg args[] = {
    { 0,    1,  default_source_mute, "%s",              NULL                          },
    { 0,    2,  default_sink_mute,   " | %s",           NULL                          },
    { 0,    3,  default_sink_volume, " %s%%",           NULL                          },
    { 0,    4,  backlight_perc,      " | \uf5de %s%%",  "intel_backlight"             },
    { 60,   0,  battery_perc,        " | \uf240 %s%%",  "BAT1",                       },
    { 155,  5,  wifi_essid,          " | \uf1eb %s",    "wlan0"                       },
    { 0,    6,  sway_keymap,         " | \uf80b %s",    NULL                          },
    { 1,    0,  datetime,            " | %s\n",         "\ue383 %T | \uf073 %d.%m.%Y" },
};
