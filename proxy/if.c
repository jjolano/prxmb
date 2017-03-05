#include "prx.h"
#include "if.h"

#include "../prxmb.h"

void* if_act0[3] = {
	(void*) if_action, 0, 0
};

void if_init(int view)
{
	paf_setInterface(view, 0x41435430 /* ACT0 */, if_act0);
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
	return 0;
}

void if_action(const char* action)
{
	prxmb_action_call(action);
}
