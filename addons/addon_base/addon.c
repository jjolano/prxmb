#include "addon.h"

SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);
SYS_MODULE_INFO(PRXMB_ADDON, 0, 0, 1);

SYS_LIB_DECLARE_WITH_STUB(PRXMB_ADDON, SYS_LIB_AUTO_EXPORT | SYS_LIB_WEAK_IMPORT, libprxmb_addon_prx);

SYS_LIB_EXPORT(prxmb_addon_action_call, PRXMB_ADDON);

bool __attribute__((weak)) prxmb_addon_action_call(const char* action)
{
	return false;
}

inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

void finalize_module(void)
{
	uint64_t meminfo[5];
	
	sys_prx_id_t prx = sys_prx_get_my_module_id();

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(482, prx, 0, (uintptr_t) meminfo);
}

void prx_unload(void)
{
	sys_prx_id_t prx = sys_prx_get_my_module_id();
	system_call_3(483, prx, 0, NULL);
}

int prx_stop(void)
{
	finalize_module();
	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}

int prx_exit(void)
{
	prx_unload();
	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}

int prx_start(size_t args, void* argv)
{
	_sys_ppu_thread_exit(0);
	return SYS_PRX_START_OK;
}
