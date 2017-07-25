#include "nettool.h"

int get_ip_num()
{
	struct ifaddrs *ifAddrStruct;
	void *tmpAddrPtr;
	char ip[INET_ADDRSTRLEN];
	int n = 0;
	getifaddrs(&ifAddrStruct);

	while (ifAddrStruct != NULL) {
		if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
			if (strcmp(ip, "127.0.0.1") != 0) {
				n++;
			}
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
	}

	//free ifaddrs
	freeifaddrs(ifAddrStruct);
	
	return n;
}

int get_local_ip(char *ips) 
{
        struct ifaddrs *ifAddrStruct;
        void *tmpAddrPtr=NULL;
        char ip[INET_ADDRSTRLEN];
        int n = 0;
        getifaddrs(&ifAddrStruct);
		
        while (ifAddrStruct != NULL) {
		if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
             //    	printf("%s IP Address:%s\n", ifAddrStruct->ifa_name, ip);

			if (n == 0)
			{
				strncat(ips, ip, INET_ADDRSTRLEN);
			} 
			else 
			{
				strncat(ips, ",", 1);
				strncat(ips, ip, INET_ADDRSTRLEN);
			}
		
			n++;
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
        }
		
        //free ifaddrs
        freeifaddrs(ifAddrStruct);
		
        return 0;
}


