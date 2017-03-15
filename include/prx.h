#pragma once

#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>

#include <cell.h>

#include "prxmb.h"
#include "cobra/storage.h"
#include "vsh_exports.h"

struct XMBAction
{
	char name[32];
	action_callback callback;
};

void parse_command_string(char command_name[32], char* command_param, char* to_parse);
bool file_exists(const char* path);
bool str_startswith(const char* str, const char* sub);

struct XMBAction* prxmb_action_find(const char name[32]);

inline void _sys_ppu_thread_exit(uint64_t val);
void finalize_module(void);
void prx_unload(void);
int prx_stop(void);
int prx_exit(void);
void prx_main(uint64_t ptr);
int prx_start(size_t args, void* argv);
