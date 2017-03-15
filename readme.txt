prxmb.sprx - main vsh plugin. should be the first entry in boot_plugins.txt
prxmb_proxy.sprx - system module proxy plugin. wM(M) and sM compatible - copy to:
- /dev_hdd0/tmp/prxmb_proxy.sprx
- /dev_hdd0/tmp/wm_res/wm_proxy.sprx (optional, but maybe it could be required in the future - if prxmb is acknowledged)
- /dev_blind/vsh/module/idle_plugin.sprx (optional, but it's more for CFW devs that want to include prxmb by default)

developer functions are available in prxmb.h, link your own project to libprxmb_prx.a
