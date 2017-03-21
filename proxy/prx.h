#pragma once

#include <sys/prx.h>
#include <sys/types.h>
#include <inttypes.h>

#include "if.h"
#include "../prxmb.h"

void prxmb_if_action(const char* action);
int prx_start(size_t args, void* argp);
int prx_stop(void);
int prx_exit(void);

extern uint32_t paf_0A1DC401(int view, int interface, void* handler); // plugin_setInterface
#define plugin_setInterface paf_0A1DC401

extern uint32_t paf_3F7CB0BF(int view, int interface, void* handler); // plugin_setInterface2
#define plugin_setInterface2 paf_3F7CB0BF

extern sys_prx_id_t sysPrxForUser_E0998DBF(const char* name,          // sys_prx_get_module_id_by_name()
                                           sys_prx_flags_t flags,
                                           sys_prx_get_module_id_by_name_option_t *pOpt);
#define sys_prx_get_module_id_by_name sysPrxForUser_E0998DBF
