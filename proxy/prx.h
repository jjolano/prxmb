#pragma once

#include <sys/prx.h>
#include <sys/types.h>
#include <inttypes.h>

#include "libc.h"
#include "if.h"

int prx_start(size_t args, void* argp);
int prx_stop(void);
int prx_exit(void);

void* getNIDfunc(const char* vsh_module, uint32_t fnid);

int paf_setInterface(int view, int interface, void* handler);
int paf_setInterface2(int view, int interface, void* handler);
