#pragma once

#include <stdint.h>
#include <sys/syscall.h>

static inline int sys_map_path(const char *oldpath, char *newpath)
{
	system_call_2(35, (uint64_t)(uint32_t)oldpath, (uint64_t)(uint32_t)newpath);
	return (int)p1;
}
