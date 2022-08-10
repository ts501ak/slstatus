/* See LICENSE file for copyright and license details. */
#include <stdint.h>

extern char buf[1024];

#define LEN(x) (sizeof (x) / sizeof *(x))

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#ifdef uninitialized_var 
#elif defined(__clang__)
#define uninitialized_var(x) x = *(&(x))
#elif defined(__GNUC__)
#define uninitialized_var(x) x = x
#else
#define uninitialized_var(x) x
#endif

extern char *argv0;

void warn(const char *, ...);
void die(const char *, ...);

int esnprintf(char *str, size_t size, const char *fmt, ...);
const char *bprintf(const char *fmt, ...);
const char *fmt_human(uintmax_t num, int base);
int pscanf(const char *path, const char *fmt, ...);
