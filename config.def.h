/* See LICENSE file for copyright and license details. */

/* base interval between updates (in ms) */
const unsigned int interval = 1000;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/*
 * function              description                            argument (example)
 *
 * battery_perc          battery percentage                     battery name (BAT0)
 *                                                              NULL on OpenBSD/FreeBSD
 * battery_state         battery charging state                 battery name (BAT0)
 *                                                              NULL on OpenBSD/FreeBSD
 * battery_remaining     battery remaining HH:MM                battery name (BAT0)
 *                                                              NULL on OpenBSD/FreeBSD
 * cpu_perc              cpu usage in percent                   NULL
 * cpu_freq              cpu frequency in MHz                   NULL
 * datetime              date and time                          format string (%F %T)
 * disk_free             free disk space in GB                  mountpoint path (/)
 * disk_perc             disk usage in percent                  mountpoint path (/)
 * disk_total            total disk space in GB                 mountpoint path (/")
 * disk_used             used disk space in GB                  mountpoint path (/)
 * entropy               available entropy                      NULL
 * gid                   GID of current user                    NULL
 * hostname              hostname                               NULL
 * ipv4                  IPv4 address                           interface name (eth0)
 * ipv6                  IPv6 address                           interface name (eth0)
 * kernel_release        `uname -r`                             NULL
 *                                                              see keyboard_indicators.c
 * load_avg              load average                           NULL
 * netspeed_rx           receive network speed                  interface name (wlan0)
 * netspeed_tx           transfer network speed                 interface name (wlan0)
 * num_files             number of files in a directory         path
 *                                                              (/home/foo/Inbox/cur)
 * ram_free              free memory in GB                      NULL
 * ram_perc              memory usage in percent                NULL
 * ram_total             total memory size in GB                NULL
 * ram_used              used memory in GB                      NULL
 * run_command           custom shell command                   command (echo foo)
 * separator             string to echo                         NULL
 * swap_free             free swap in GB                        NULL
 * swap_perc             swap usage in percent                  NULL
 * swap_total            total swap size in GB                  NULL
 * swap_used             used swap in GB                        NULL
 * temp                  temperature in degree celsius          sensor file
 *                                                              (/sys/class/thermal/...)
 *                                                              NULL on OpenBSD
 *                                                              thermal zone on FreeBSD
 *                                                              (tz0, tz1, etc.)
 * uid                   UID of current user                    NULL
 * uptime                system uptime                          NULL
 * username              username of current user               NULL
 * vol_perc              OSS/ALSA volume in percent             mixer file (/dev/mixer)
 *                                                              NULL on OpenBSD
 * wifi_perc             WiFi signal in percent                 interface name (wlan0)
 * wifi_essid            WiFi ESSID                             interface name (wlan0)
 *
 * default_sink_volume   volume of default sink in percent      NULL
 * default_sink_mute     state of mute from the default sink    NULL 
 * default_source_volume volume of default source in percent    NULL
 * default_source_mute   state of mute from the default source  NULL
 *
 * sway_keymap           active keyboard layout in swaywm       NULL
 *                       (assumes all devices have the 
 *                       same active layout)
 *
 * backlight_perc        brightness of a backlight in percent   backlight (intel_backlight) 
 */

 /*  
 * interval defines the update time as a multiple of the base interval (by default 1000 ms) 
 * interval of 0 zero disables updating
 * signals can be triggered by running "kill -${signal} $(pidof slstatus)"
 * replace ${signal} with 34 + the assigned signal value, therefore the minimum is 35
 * a signal value of 0 corresponds to no update signal
 */
static const struct arg args[] = {
    /*interval  signal  function    format  argument */
    { 1,        0,      datetime,   "%s",   "%F %T" },
};
