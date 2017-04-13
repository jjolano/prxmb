#include "prx.h"
#include "pftutils.h"
#include "cobra/cobra.h"
#include "cobra/storage.h"

SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);
SYS_MODULE_INFO(PRXMB, SYS_MODULE_ATTR_EXCLUSIVE_LOAD | SYS_MODULE_ATTR_EXCLUSIVE_START, 0, 1);

SYS_LIB_DECLARE_WITH_STUB(PRXMB, SYS_LIB_AUTO_EXPORT | SYS_LIB_WEAK_IMPORT, libprxmb_prx);

SYS_LIB_EXPORT(prxmb_action_hook, PRXMB);
SYS_LIB_EXPORT(prxmb_action_unhook, PRXMB);
SYS_LIB_EXPORT(prxmb_action_call, PRXMB);

struct PTTree* xmbactions = NULL;

sys_ppu_thread_t prx_tid;
bool redirect;

bool file_exists(const char* path)
{
	CellFsStat st;
	return cellFsStat(path, &st) == 0;
}

int prxmb_action_hook(const char name[MAX_ACT_NAMELEN], action_callback callback)
{
	if(xmbactions == NULL)
	{
		xmbactions = pttree_create();
	}

	if(pttree_search(xmbactions, name) != NULL)
	{
		return -1;
	}

	pttree_insert(xmbactions, name, callback);
	return 0;
}

void prxmb_action_unhook(const char name[MAX_ACT_NAMELEN])
{
	struct PTNode* n;

	if(xmbactions != NULL && (n = pttree_search(xmbactions, name)) != NULL)
	{
		n->data_ptr = NULL;
	}
}

void prxmb_action_call(const char* action)
{
	// Load PRXMB addons and call action handlers
	bool handled = false;
	int fd;

	if(cellFsOpendir(ADDON_DIR, &fd) == 0)
	{
		CellFsDirent dirent;
		uint64_t nread;

		char* filename = malloc(0x420 * sizeof(char));

		while(!handled && cellFsReaddir(fd, &dirent, &nread) == 0 && nread > 0)
		{
			// Load addon using COBRA
			sprintf(filename, ADDON_DIR "/%s", dirent.d_name);

			char* filename_ext = strrchr(filename, '.');

			if(filename_ext != NULL && strcmp(filename_ext, ".sprx") == 0)
			{
				int ret = cobra_load_vsh_plugin(0, filename, NULL, 0);

				if(ret == 0 && prxmb_addon_running())
				{
					// call function
					if(prxmb_addon_action_call(action))
					{
						handled = true;
					}

					cobra_unload_vsh_plugin(0);
				}
			}
		}

		free(filename);

		cellFsClosedir(fd);
	}

	if(handled)
	{
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
		vshtask_notify("Warning: PRXMB action name exceeds maximum length.");
		namelen = MAX_ACT_NAMELEN;
	}

	strncpy(name, action, namelen);
	name[namelen] = '\0';

	struct PTNode* n = (xmbactions != NULL) ? pttree_search(xmbactions, name) : NULL;

	if(n != NULL)
	{
		action_callback callback = n->data_ptr;

		if(callback != NULL)
		{
			(*callback)(name, params);
		}
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

void prxmb_free(void)
{
	if(xmbactions != NULL)
	{
		pttree_destroy(xmbactions);
		xmbactions = NULL;
	}

	if(redirect)
	{
		sys_map_path(VSHMODULE_SPRX, NULL);
		redirect = false;
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
	prxmb_free();
	finalize_module();
	
	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}

int prx_exit(void)
{
	prxmb_free();
	prx_unload();

	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}

int prx_start(size_t args, void* argv)
{
	redirect = file_exists(PRXMB_PROXY_SPRX);

	if(redirect)
	{
		sys_map_path(VSHMODULE_SPRX, (char*) PRXMB_PROXY_SPRX);
	}

	_sys_ppu_thread_exit(0);
	return SYS_PRX_START_OK;
}
