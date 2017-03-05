#ifndef __PRXMB_INC__
#define __PRXMB_INC__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*action_callback)(const char[32] /* action name */, const char* /* params */);

/*
 * Hook a function for specified action on proxy plugin.
 * Returns 0 if successfully hooked, or -1 if action was already hooked.
 */
int prxmb_action_hook(const char name[32], action_callback callback);
void prxmb_action_unhook(const char name[32]);

/*
 * Internally used by proxy plugin to call hooks.
 */
void prxmb_action_call(const char* action);

#ifdef __cplusplus
}
#endif

#endif
