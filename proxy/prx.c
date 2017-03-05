#include "prx.h"

SYS_MODULE_INFO(prxmb_proxy, 0, 0, 1);
SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);

void* getNIDfunc(const char* vsh_module, uint32_t fnid)
{
	uint32_t table = (*(uint32_t*)0x1008C) + 0x984; // vsh table address

	while(((uint32_t)*(uint32_t*)table) != 0)
	{
		uint32_t* export_stru_ptr = (uint32_t*)*(uint32_t*)table;
		const char* lib_name_ptr =  (const char*)*(uint32_t*)((char*)export_stru_ptr + 0x10);

		if(strncmp(vsh_module, lib_name_ptr, strlen(lib_name_ptr)) == 0)
		{
			uint32_t lib_fnid_ptr = *(uint32_t*)((char*)export_stru_ptr + 0x14);
			uint32_t lib_func_ptr = *(uint32_t*)((char*)export_stru_ptr + 0x18);
			uint16_t count = *(uint16_t*)((char*)export_stru_ptr + 6); // number of exports

			for(int i = 0; i < count; i++)
			{
				if(fnid == *(uint32_t*)((char*)lib_fnid_ptr + i*4))
				{
					return (void**)*((uint32_t*)(lib_func_ptr) + i);
				}
			}
		}
		
		table += 4;
	}

	return 0;
}

int paf_setInterface(int view, int interface, void* handler)
{
	int (*plugin_SetInterface)(int v, int i, void* h) = getNIDfunc("paf", 0xA1DC401);

	if(plugin_SetInterface)
	{
		return plugin_SetInterface(view, interface, handler);
	}

	return -1;
}

int paf_setInterface2(int view, int interface, void* handler)
{
	int (*plugin_SetInterface2)(int v, int i, void* h) = getNIDfunc("paf", 0x3F7CB0BF);

	if(plugin_SetInterface2)
	{
		return plugin_SetInterface2(view, interface, handler);
	}

	return -1;
}

int prx_start(size_t args, void *argp)
{
	void *if_proxy_func[4] =
	{
		(void*) if_init,
		(int* ) if_start,
		(int* ) if_stop,
		(int* ) if_exit
	};

	paf_setInterface2(*(unsigned int*) argp, 1, (void*) if_proxy_func);
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
