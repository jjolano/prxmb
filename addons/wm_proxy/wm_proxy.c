// webMAN/webMAN MOD compatibility module for PRXMB

#include "addon.h"
#include "wm_proxy.h"
#include "types.h"

bool str_startswith(const char* str, const char* sub)
{
	return strncmp(str, sub, strlen(sub)) == 0;
}

bool prxmb_addon_action_call(const char* action)
{
	// wm_proxy compatibility
	if(str_startswith(action, "/") || strcmp(action, "sman") == 0)
	{
		// pass control to wm_proxy
		wm_plugin_action(action);
		return true;
	}

	if(str_startswith(action, "http://127.0.0.1/"))
	{
		// pass control to wm_proxy
		wm_plugin_action(action + 16);
		return true;
	}

	return false;
}

void wm_plugin_action(const char* action)
{
	// Connect to webMAN
	int wm_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in wm_addr;

	wm_addr.sin_family = AF_INET;
	wm_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	wm_addr.sin_port = htons(80);

	struct timeval tv;

	tv.tv_usec = 0;
	tv.tv_sec = 3;

	setsockopt(wm_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	if(wm_sock <= 0 || connect(wm_sock, (struct sockaddr*) &wm_addr, sizeof(wm_addr)) != 0)
	{
		vshtask_notify("wm_proxy: Failed to communicate with webMAN.");
		return;
	}

	// Generate HTTP GET request
	char* action_temp = strdup(action);
	char* buffer = (char*) malloc((6 + strlen(action)) * sizeof(char));
	char* token = strtok(action_temp, " ");

	if(token == NULL)
	{
		// no need to replace
		sprintf(buffer, "GET %s", action_temp);
	}
	else
	{
		strcpy(buffer, "GET ");

		// replace spaces with %20
		do {
			strcat(buffer, token);
			token = strtok(NULL, " ");

			if(token != NULL)
			{
				strcat(buffer, "%20");
			}

		} while(token != NULL);
	}

	strcat(buffer, "\r\n");

	// Send request to socket
	if(send(wm_sock, buffer, strlen(buffer), 0) <= 0)
	{
		vshtask_notify("wm_proxy: Failed to communicate with webMAN.");
	}

	shutdown(wm_sock, SHUT_RDWR);
	socketclose(wm_sock);

	free(buffer);
	free(action_temp);
}
