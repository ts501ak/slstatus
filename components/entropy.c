/* See LICENSE file for copyright and license details. */
#include <stdint.h>
#include <stdio.h>

#include "../util.h"

const char *
entropy(void)
{
	uintmax_t num;

	if (pscanf("/proc/sys/kernel/random/entropy_avail", "%ju", &num)
	    != 1) {
		return NULL;
	}

	return bprintf("%ju", num);
}
