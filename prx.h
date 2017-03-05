#pragma once

#include <stdbool.h>
#include <inttypes.h>

#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>

#include "vsh_exports.h"
#include "prxmb.h"

struct XMBAction
{
	char name[32];
	action_callback callback;
};

void* getNIDfunc(const char* vsh_module, uint32_t fnid);

void vshtask_showMessage(const char* msg);

void parse_command_string(char command_name[32], char* command_param, char* to_parse);
struct XMBAction* prxmb_action_find(const char name[32]);

inline void _sys_ppu_thread_exit(uint64_t val);
void finalize_module(void);
void prx_unload(void);
int prx_stop(void);
int prx_exit(void);
void prx_main(uint64_t ptr);
int prx_start(size_t args, void* argv);
