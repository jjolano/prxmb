#include <sdk_version.h>
#include <cellstatus.h>
#include <sys/prx.h>
#include <cell/cell_fs.h>

#include <stdbool.h>

#include "prxmb_prx.h"
#include "cobra_syscalls.h"
#include "pftutils.h"
#include "common/vsh_exports/vsh_exports.h"

SYS_MODULE_INFO(prxmb, 0, 2, 0);
SYS_MODULE_START(_start);
SYS_MODULE_STOP(_stop);

SYS_LIB_DECLARE_WITH_STUB(prxmb, SYS_LIB_AUTO_EXPORT | SYS_LIB_WEAK_IMPORT, libprxmb_prx);

SYS_LIB_EXPORT(prxmb_action_hook, prxmb);
SYS_LIB_EXPORT(prxmb_action_unhook, prxmb);
SYS_LIB_EXPORT(_prxmb_action_call, prxmb);

int _start(void);
int _stop(void);
inline bool _file_exists(const char* path);
inline void _prxmb_unload(void);
inline void _sys_ppu_thread_exit(uint64_t val);
inline void _stop_cobra(void);
inline void _unload(void);

static struct PTTree* module_actions = NULL;
static bool module_redirect = false;

// Export Functions
void prxmb_action_hook(const char* action_name, action_callback callback)
{
	if(module_actions != NULL)
	{
		pttree_insert(module_actions, action_name, callback);
	}
}

void prxmb_action_unhook(const char* action_name)
{
	struct PTNode* n;

	if(module_actions != NULL && (n = pttree_search(module_actions, action_name)) != NULL)
	{
		n->data_ptr = NULL;
	}
}

void _prxmb_action_call(const char* action)
{
	bool handled = false;

	if(!handled)
	{
		extern bool wm_proxy_action(const char* action);
		handled = wm_proxy_action(action);
	}

	if(!handled && module_actions != NULL)
	{
		char* action_params = strchr((char*) action, ' ');
		int namelen = action_params != NULL ? action_params - action : strlen(action);
		char* action_name = malloc(namelen * sizeof(char));

		strncpy(action_name, action, namelen);
		action_name[namelen] = '\0';

		struct PTNode* n = pttree_search(module_actions, action_name);

		if(n != NULL)
		{
			action_callback callback = n->data_ptr;

			if(callback != NULL)
			{
				(*callback)(action_name, action_params != NULL ? (action_params + 1) : NULL);
			}
		}
		else
		{
			vshtask_notify(action_name);
		}

		free(action_name);
	}
}

// PRX Functions
inline bool _file_exists(const char* path)
{
	CellFsStat st;
	return cellFsStat(path, &st) == 0;
}

inline void _prxmb_unload(void)
{
	if(module_actions != NULL)
	{
		pttree_destroy(module_actions);
		module_actions = NULL;
	}

	if(module_redirect)
	{
		sys_map_path(VSH_MODULE, NULL);
		module_redirect = false;
	}
}

inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

inline void _stop_cobra(void)
{
	uint64_t meminfo[5];
	
	sys_prx_id_t prx = sys_prx_get_my_module_id();

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(482, prx, 0, (uintptr_t) meminfo);
}

int _start(void)
{
	module_actions = pttree_create();
	module_redirect = _file_exists(PRXMB_MODULE);

	if(module_redirect)
	{
		sys_map_path(VSH_MODULE, (char*) PRXMB_MODULE);
	}

	extern void dev_blind_init(void);
	dev_blind_init();

	_sys_ppu_thread_exit(0);
	return SYS_PRX_RESIDENT;
}

int _stop(void)
{
	_prxmb_unload();
	_stop_cobra();
	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}
