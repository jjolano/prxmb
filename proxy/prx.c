#include "prx.h"

SYS_MODULE_INFO(prxmb_proxy, 0, 0, 1);
SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);

void* if_proxy_func[4] =
{
	(void*) if_init,
	(int* ) if_start,
	(int* ) if_stop,
	(int* ) if_exit
};

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
