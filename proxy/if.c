#include "prx.h"
#include "if.h"

#include "../prxmb.h"

void* if_act0[3] = { if_action, 0, 0 };

void if_init(int view)
{
	plugin_setInterface(view, ACT0, if_act0);
}

int if_start(void* view)
{
	return SYS_PRX_START_OK;
}

int if_stop(void)
{
	return SYS_PRX_STOP_OK;
}

int if_exit(void)
{
	return SYS_PRX_STOP_OK;
}

void if_action(const char* action)
{
	sys_prx_id_t prxmb = sys_prx_get_module_id_by_name("PRXMB", 0, 0);

	if(prxmb > 0)
	{
		prxmb_action_call(action);
	}
}
