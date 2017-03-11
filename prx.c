#include "prx.h"

SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);
SYS_MODULE_INFO(PRXMB, 0, 0, 1);

SYS_LIB_DECLARE_WITH_STUB(PRXMB, SYS_LIB_AUTO_EXPORT, libprxmb_prx);

SYS_LIB_EXPORT(prxmb_action_hook, PRXMB);
SYS_LIB_EXPORT(prxmb_action_unhook, PRXMB);
SYS_LIB_EXPORT(prxmb_action_call, PRXMB);

struct XMBAction* xmbactions = NULL;
int xmbactions_count = 0;

sys_ppu_thread_t prx_tid;
bool prx_running = false;

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

void vshtask_showMessage(const char* msg)
{
	int (*vshtask_notify)(int, const char*) = getNIDfunc("vshtask", 0xA02D46E7);

	if(vshtask_notify)
	{
		vshtask_notify(0, msg);
	}
}

void parse_command_string(char command_name[32], char* command_param, char* to_parse)
{
	// initialize output
	command_name[0] = '\0';
	command_param[0] = '\0';

	char* token = strtok(to_parse, " ");

	if(token != NULL)
	{
		strcpy(command_name, token);
	}

	token = strtok(NULL, " ");

	while(token != NULL)
	{
		if(command_param[0] != '\0')
		{
			strcat(command_param, " ");
		}

		strcat(command_param, token);

		token = strtok(NULL, " ");
	}
}

bool file_exists(const char* path)
{
	CellFsStat st;
	
	if(cellFsStat(path, &st) == 0)
	{
		return true;
	}
	
	return false;
}

struct XMBAction* prxmb_action_find(const char name[32])
{
	int i;
	for(i = 0; i < xmbactions_count; ++i)
	{
		struct XMBAction* xmbaction = &xmbactions[i];

		if(strcmp(xmbaction->name, name) == 0)
		{
			return xmbaction;
		}
	}

	return NULL;
}

int prxmb_action_hook(const char name[32], action_callback callback)
{
	if(prxmb_action_find(name) != NULL)
	{
		return -1;
	}

	xmbactions = (struct XMBAction*) realloc(xmbactions, ++xmbactions_count * sizeof(struct XMBAction));

	struct XMBAction* xmbaction = &xmbactions[xmbactions_count - 1];
	
	strcpy(xmbaction->name, name);
	xmbaction->callback = callback;

	return 0;
}

void prxmb_action_unhook(const char name[32])
{
	int i;
	for(i = 0; i < xmbactions_count; ++i)
	{
		struct XMBAction* xmbaction = &xmbactions[i];

		if(strcmp(xmbaction->name, name) == 0)
		{
			break;
		}
	}

	if(i == xmbactions_count)
	{
		return;
	}

	memmove(&xmbactions[i], &xmbactions[i + 1], (xmbactions_count - i - 1) * sizeof(struct XMBAction));
	xmbactions = (struct XMBAction*) realloc(xmbactions, --xmbactions_count * sizeof(struct XMBAction));
}

void prxmb_action_call(const char* action)
{
	char* action_temp = strdup(action);

	char name[32];
	char* params = (char*) malloc(1024 * sizeof(char));

	parse_command_string(name, params, action_temp);

	struct XMBAction* xmbaction = prxmb_action_find(name);

	if(xmbaction != NULL)
	{
		(*xmbaction->callback)(name, params);
	}
	else
	{
		char msg[256];
		int len = snprintf(msg, 255, "PRXMB unhandled action: %s (params: %s)", name, params);
		msg[len] = '\0';

		vshtask_showMessage(msg);
	}

	free(params);
	free(action_temp);
}

inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

void finalize_module(void)
{
	uint64_t meminfo[5];

	sys_prx_id_t prx = sys_prx_get_module_id_by_address(finalize_module);

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(482, prx, 0, (uintptr_t) meminfo);
}

void prx_unload(void)
{
	sys_prx_id_t prx = sys_prx_get_module_id_by_address(prx_unload);
	system_call_3(483, prx, 0, NULL);
}

int prx_stop(void)
{
	if(prx_running)
	{
		prx_running = false;

		uint64_t prx_exitcode;
		sys_ppu_thread_join(prx_tid, &prx_exitcode);
	}

	finalize_module();
	_sys_ppu_thread_exit(SYS_PRX_STOP_OK);
	return SYS_PRX_STOP_OK;
}

int prx_exit(void)
{
	if(prx_running)
	{
		prx_running = false;

		uint64_t prx_exitcode;
		sys_ppu_thread_join(prx_tid, &prx_exitcode);
	}

	prx_unload();
	_sys_ppu_thread_exit(SYS_PRX_STOP_OK);
	return SYS_PRX_STOP_OK;
}

void prx_main(uint64_t ptr)
{
	prx_running = true;

	if(file_exists(PRXMB_PROXY_SPRX))
	{
		sys_map_path(VSHMODULE_SPRX, PRXMB_PROXY_SPRX);
	}

	while(prx_running)
	{
		sys_timer_sleep(1);
	}

	sys_map_path(VSHMODULE_SPRX, NULL);

	xmbactions_count = 0;
	free(xmbactions);

	// wait for other plugins to exit
	sys_timer_sleep(1);

	if(prx_running)
	{
		prx_running = false;
		finalize_module();
	}
	
	sys_ppu_thread_exit(0);
}

int prx_start(size_t args, void* argv)
{
	sys_ppu_thread_create(&prx_tid, prx_main, 0, 1000, 0x1000, SYS_PPU_THREAD_CREATE_JOINABLE, (char*) "prxmb");
	_sys_ppu_thread_exit(SYS_PRX_START_OK);
	return SYS_PRX_START_OK;
}
