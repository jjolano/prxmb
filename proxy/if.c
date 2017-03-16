#include "prx.h"
#include "if.h"

#include "../prxmb.h"

void* if_act0[1] = { if_action };
void* if_mod0 = 0;

void if_init(int view)
{
	plugin_setInterface(view, PRX_ACT0, if_act0);
	plugin_setInterface(view, PRX_MOD0, if_mod0);
}

int if_start(void* view)
{
	return SYS_PRX_START_OK;
}

int if_stop(void)
{
	return SYS_PRX_STOP_OK;
}

void if_exit(void)
{
	
}

void if_action(const char* action)
{
	prxmb_action_call(action);
}
