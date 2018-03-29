#include <stdbool.h>
#include <sys/syscall.h>

#include "prxmb/prxmb_prx.h"
#include "common/vsh_exports/vsh_exports.h"

void dev_blind_mount(const char* action_name, const char* action_params);
void dev_blind_unmount(const char* action_name, const char* action_params);
void dev_blind_init(void);

void dev_blind_mount(const char* action_name, const char* action_params)
{
	system_call_8(837,
		(uint64_t) "CELL_FS_IOS:BUILTIN_FLSH1",
		(uint64_t) "CELL_FS_FAT",
		(uint64_t) "/dev_blind",
		0, 0, 0, 0, 0);
}

void dev_blind_unmount(const char* action_name, const char* action_params)
{
	system_call_1(838, (uint64_t) "/dev_blind");
}

void dev_blind_init(void)
{
	prxmb_action_hook("dev_blind_mount", dev_blind_mount);
	prxmb_action_hook("dev_blind_unmount", dev_blind_unmount);
}
