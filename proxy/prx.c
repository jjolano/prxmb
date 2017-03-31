#include "prx.h"

SYS_MODULE_INFO(prxmb_proxy, 0, 0, 1);
SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);

void* if_proxy_func[4] = { if_init, if_start, if_stop, if_exit };

void prxmb_if_action(const char* action)
{
	if(prxmb_running())
	{
		prxmb_action_call(action);
	}
	else
	{
		vshtask_notify("Error: PRXMB is not running.");
	}
}

int prx_start(size_t args, void* argp)
{
	plugin_setInterface2(*(unsigned int*) argp, 1, if_proxy_func);
	return SYS_PRX_RESIDENT;
}

int prx_stop(void)
{
	return SYS_PRX_STOP_OK;
}

int prx_exit(void)
{
	return 0;
}
