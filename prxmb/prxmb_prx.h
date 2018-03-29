#pragma once

#define prxmb_running() (sys_prx_get_module_id_by_name("prxmb", 0, NULL) > 0)

#define VSH_MODULE		"/dev_flash/vsh/module/idle_plugin.sprx"
#define PRXMB_MODULE	"/dev_hdd0/prxmb/prxmb_plugin.sprx"

typedef void (*action_callback)(const char* action_name, const char* action_params);

void prxmb_action_hook(const char* action_name, action_callback callback);
void prxmb_action_unhook(const char* action_name);
void _prxmb_action_call(const char* action);
