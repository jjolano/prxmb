#pragma once

#define PRX_ACT0	0x41435430
#define PRX_MOD0	0x4D4F4430

void if_init(int view);
int if_start(void* view);
int if_stop(void);
void if_exit(void);
void if_action(const char* action);