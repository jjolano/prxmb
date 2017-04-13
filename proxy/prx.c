#include "prx.h"
#include "cobra/cobra.h"

#include <cell.h>

SYS_MODULE_INFO(prxmb_proxy, 0, 0, 2);
SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_exit);

void* if_proxy_func[4] = { if_init, if_start, if_stop, if_exit };

void prxmb_if_action(const char* action)
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
					handled = prxmb_addon_action_call(action);
					cobra_unload_vsh_plugin(0);
				}
			}
		}

		free(filename);

		cellFsClosedir(fd);
	}

	if(!handled)
	{
		if(prxmb_running())
		{
			prxmb_action_call(action);
		}
		else
		{
			vshtask_notify("PRXMB is not running.");
		}
	}
}

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
