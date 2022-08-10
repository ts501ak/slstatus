#include <limits.h>
#include <stddef.h>

#include "../util.h"


const char *
backlight_perc(const char *backlight) 
{
  int perc;
  float max_brightness, brightness;
  char path[PATH_MAX];
  
  if (esnprintf(path, sizeof(path),
                "/sys/class/backlight/%s/max_brightness", backlight) < 0) 
    return NULL;
  
  if (pscanf(path, "%d", &max_brightness) != 1) 
    return NULL;
  
  if (esnprintf(path, sizeof(path),
                "/sys/class/backlight/%s/brightness", backlight) < 0)
    return NULL;
  
  if (pscanf(path, "%d", &brightness) != 1) 
    return NULL;
  
  perc = brightness / max_brightness * 100;
  return bprintf("%d", perc);
}
