#ifndef _NET_TOOL_H_
#define _NET_TOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int get_ip_num();
//int get_local_ip(char *ip_list);
int get_local_ip(char *ips);

#ifdef __cplusplus
}
#endif


#endif
