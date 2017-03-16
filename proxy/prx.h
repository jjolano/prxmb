#pragma once

#include <sys/prx.h>
#include <sys/types.h>
#include <inttypes.h>

#include "if.h"

int prx_start(size_t args, void* argp);
int prx_stop(void);
int prx_exit(void);

extern uint32_t paf_0A1DC401(int view, int interface, void* handler); // plugin_setInterface
#define plugin_setInterface paf_0A1DC401

extern uint32_t paf_3F7CB0BF(int view, int interface, void* handler); // plugin_setInterface2
#define plugin_setInterface2 paf_3F7CB0BF
