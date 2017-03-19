#include "prx.h"

#include "compat/wm_proxy/wm_proxy.h"

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

bool file_exists(const char* path)
{
	CellFsStat st;
	return cellFsStat(path, &st) == 0;
}

bool str_startswith(const char* str, const char* sub)
{
	return strncmp(str, sub, strlen(sub)) == 0;
}

struct XMBAction* prxmb_action_find(const char name[MAX_ACT_NAMELEN])
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

int prxmb_action_hook(const char name[MAX_ACT_NAMELEN], action_callback callback)
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

void prxmb_action_unhook(const char name[MAX_ACT_NAMELEN])
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
	// wm_proxy compatibility
	if(str_startswith(action, "/") || strcmp(action, "sman") == 0)
	{
		// pass control to wm_proxy
		wm_plugin_action(action);
		return;
	}

	if(str_startswith(action, "http://127.0.0.1/"))
	{
		// pass control to wm_proxy
		wm_plugin_action(action + 16);
		return;
	}

	char name[MAX_ACT_NAMELEN];
	char* params = strchr((char*) action, ' ');
	int namelen = 0;

	if(params != NULL)
	{
		namelen = params - action;
		params++;
	}
	else
	{
		namelen = strlen(action);
	}

	if(namelen > MAX_ACT_NAMELEN)
	{
		// warning
		vshtask_notify("PRXMB warning: action name exceeds maximum length");
		namelen = MAX_ACT_NAMELEN;
	}

	strncpy(name, action, namelen);
	name[namelen] = '\0';

	struct XMBAction* xmbaction = prxmb_action_find(name);

	if(xmbaction != NULL)
	{
		(*xmbaction->callback)(name, params);
	}
	else
	{
		char* msg = (char*) malloc((strlen(action) + 32) * sizeof(char));

		if(params == NULL)
		{
			sprintf(msg, "PRXMB unhandled action: %s", name);
		}
		else
		{
			sprintf(msg, "PRXMB unhandled action: %s (params: %s)", name, params);
		}

		vshtask_notify(msg);
		free(msg);
	}
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
	bool redirect = false;

	prx_running = true;

	if(file_exists(PRXMB_PROXY_SPRX))
	{
		redirect = true;
		sys_map_path(VSHMODULE_SPRX, (char*) PRXMB_PROXY_SPRX);
	}

	while(prx_running)
	{
		sys_timer_sleep(1);
	}

	if(redirect)
	{
		sys_map_path(VSHMODULE_SPRX, NULL);
	}

	xmbactions_count = 0;
	free(xmbactions);

	// wait for other plugins to exit
	sys_timer_sleep(2);
	
	sys_ppu_thread_exit(0);
}

int prx_start(size_t args, void* argv)
{
	sys_ppu_thread_create(&prx_tid, prx_main, 0, 1001, 0x2000, SYS_PPU_THREAD_CREATE_JOINABLE, (char*) "prxmb");
	_sys_ppu_thread_exit(SYS_PRX_START_OK);
	return SYS_PRX_START_OK;
}
