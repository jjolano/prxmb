// webMAN/webMAN MOD compatibility module for PRXMB

#include "wm_proxy.h"
#include "types.h"

int connect_to_webman(void)
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0x7F000001;	//127.0.0.1 (localhost)
	sin.sin_port = htons(80);			//http port (80)
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0) return -1;

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 3;

	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		return -1;

	return s;
}

void wm_plugin_action(const char* action)
{
	int s = connect_to_webman();
	if(s >= 0)
	{
		char proxy_action[256];

		u32 i = 0;
		u32 pa = 0;

		proxy_action[pa++] = 'G';
		proxy_action[pa++] = 'E';
		proxy_action[pa++] = 'T';
		proxy_action[pa++] = ' ';

		if(action[0] != '/') i = 16;
		if(action[i] == '/')
		{
			for(;(i < strlen(action)) && (pa < 250); i++)
			{
				if(action[i] != 0x20)
					proxy_action[pa++] = action[i];
				else
				{
					proxy_action[pa++] = '%';
					proxy_action[pa++] = '2';
					proxy_action[pa++] = '0';
				}
			}

			proxy_action[pa++] = '\r';
			proxy_action[pa++] = '\n';
			proxy_action[pa] = 0;

			send(s, proxy_action, pa, 0);
		}
		else
			send(s, action, strlen(action), 0);

		shutdown(s, SHUT_RDWR);
		socketclose(s);
	}
	else vshtask_notify("Plugin not ready!");
}
