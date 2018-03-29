#pragma once

#include <sys/prx.h>

#include <stdint.h>

#include "prxmb/prxmb_prx.h"
#include "common/vsh_exports/vsh_exports.h"

void prxmb_if_action(const char* action);

extern uint32_t paf_0A1DC401(int view, int interface, void* handler); // plugin_setInterface
#define plugin_setInterface paf_0A1DC401

extern uint32_t paf_3F7CB0BF(int view, int interface, void* handler); // plugin_setInterface2
#define plugin_setInterface2 paf_3F7CB0BF
