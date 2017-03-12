prxmb.sprx - main vsh plugin, should be loaded in the first slot
prxmb_proxy.sprx - system module proxy plugin. should be installed at /dev_hdd0/tmp (or even directly in flash as xai_plugin.sprx)

developer functions are available in prxmb.h, link your own project to libprxmb_prx.a

do document any action names your sprx uses at release! this ensures that devs don't conflict in action name choices.
