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
	if(s < 0)
	{
		return -1;
	}

	struct timeval tv;
	tv.tv_usec = 0;

	tv.tv_sec = 3;
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return -1;
	}

	tv.tv_sec = 60;
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	return s;
}

void sclose(int *socket_e)
{
	//if(*socket_e != -1)
	{
		shutdown(*socket_e, SHUT_RDWR);
		socketclose(*socket_e);
		//*socket_e = -1;
	}
}

void wm_plugin_action(const char * action)
{
	int s = connect_to_webman();
	if(s >= 0)
	{
		char proxy_action[512];
		memcpy(proxy_action, "GET ", 4);

		u32 pa = 4;

		if(*action != '/') action += 16; // using http://127.0.0.1/
		if(*action == '/')
		{
			for(;*action && (pa < 505); action++)
			{
				if(*action != 0x20)
					proxy_action[pa++] = *action;
				else
				{
					memcpy(proxy_action + pa, "%20", 3); pa += 3;
				}
			}

			memcpy(proxy_action + pa, "\r\n\0", 3); pa +=2;
			send(s, proxy_action, pa, 0);
		}
		sclose(&s);
	}
	else vshtask_notify("webMAN not ready!");
}
