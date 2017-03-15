prxmb.sprx - main vsh plugin. should be the first entry in boot_plugins.txt
prxmb_proxy.sprx - system module proxy plugin. wM(M) and sM compatible - copy to:
- /dev_hdd0/tmp/prxmb_proxy.sprx
- /dev_hdd0/tmp/wm_res/wm_proxy.sprx
- /dev_blind/vsh/module/idle_plugin.sprx

developer functions are available in prxmb.h, link your own project to libprxmb_prx.a

do document any action names your sprx uses at release! this ensures that devs don't conflict in action name choices.
