#include <sdk_version.h>
#include <cellstatus.h>
#include <sys/prx.h>

#include "prx.h"
#include "mod.h"

SYS_MODULE_INFO(prxmb_plugin, 0, 1, 0);
SYS_MODULE_START(_start);
SYS_MODULE_STOP(_stop);

int _start(size_t args, void* argp);
int _stop(void);

void prxmb_if_action(const char* action)
{
	if(prxmb_running())
	{
		_prxmb_action_call(action);
	}
	else
	{
		vshtask_notify("PRXMB not available.");
	}
}

int _start(size_t args, void* argp)
{
	void* if_proxy_func[4] = { if_init, if_start, if_stop, if_exit };
	plugin_setInterface2(*(unsigned int*) argp, 1, if_proxy_func);
	return SYS_PRX_RESIDENT;
}

int _stop(void)
{
	return SYS_PRX_STOP_OK;
}
