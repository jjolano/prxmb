#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>

#include <cell.h>

#include "prxmb.h"
#include "vsh_exports.h"

bool file_exists(const char* path);
bool str_startswith(const char* str, const char* sub);

void prxmb_free(void);

inline void _sys_ppu_thread_exit(uint64_t val);
void finalize_module(void);
void prx_unload(void);
int prx_stop(void);
int prx_exit(void);
int prx_start(size_t args, void* argv);
