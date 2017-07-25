/********************************************
this file contain many usefull c/c++ functions here
*********************************************/

/**********************************************
for windows os to init socket
**********************************************/
void nitNetwork()
{
#ifdef WIN32 
   WORD wVersionRequested = MAKEWORD( 2, 2 );
   WSADATA wsaData;
   int err;
	
   err = WSAStartup( wVersionRequested, &wsaData );
   if ( err != 0 ) 
   {
      // could not find a usable WinSock DLL
      cerr << "Could not load winsock" << endl;
      assert(0); // is this is failing, try a different version that 2.2, 1.0 or later will likely work 
      exit(1);
   }
    
   /* Confirm that the WinSock DLL supports 2.2.*/
   /* Note that if the DLL supports versions greater    */
   /* than 2.2 in addition to 2.2, it will still return */
   /* 2.2 in wVersion since that is the version we      */
   /* requested.                                        */
    
   if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) 
   {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.                                  */
      WSACleanup( );
      cerr << "Bad winsock verion" << endl;
      assert(0); // is this is failing, try a different version that 2.2, 1.0 or later will likely work 
      exit(1);
   }    
#endif
}


/**********************************************
return a random number to use as a port 
**********************************************/
int stunRand()
{
   // return 32 bits of random stuff
   assert( sizeof(int) == 4 );
   static bool init=false;
   if ( !init )
   { 
	  init = true;
		
	  UInt64 tick;
		
#if defined(WIN32) 
	  volatile unsigned int lowtick=0,hightick=0;
	  __asm
		 {
			rdtsc 
			   mov lowtick, eax
			   mov hightick, edx
			   }
	  tick = hightick;
	  tick <<= 32;
	  tick |= lowtick;
#elif defined(__GNUC__) && ( defined(__i686__) || defined(__i386__) )
	  asm("rdtsc" : "=A" (tick));
#elif defined (__SUNPRO_CC) || defined( __sparc__ )	
	  tick = gethrtime();
#elif defined(__MACH__)  || defined(__linux)
	  int fd=open("/dev/random",O_RDONLY);
	  read(fd,&tick,sizeof(tick));
	  closesocket(fd);
#else
#     error Need some way to seed the random number generator 
#endif 
	  int seed = int(tick);
#ifdef WIN32
	  srand(seed);
#else
	  srandom(seed);
#endif
   }
	
#ifdef WIN32
   assert( RAND_MAX == 0x7fff );
   int r1 = rand();
   int r2 = rand();
	
   int ret = (r1<<16) + r2;
	
   return ret;
#else
   return random(); 
#endif
}

int stunRandomPort()
{
   int min=0x4000;
   int max=0x7FFF;
	
   int ret = stunRand();
   ret = ret|min;
   ret = ret&max;
	
   return ret;
}


/**********************************************
get_app_compile_date
***********************************************/
char APP_VERSION[16]= {0};
UINT32 FirmWare_Version;
void get_app_compile_date()
{
	char comp_date[16]= {0};
	char temp_mon[4]= {0};

	memcpy(temp_mon,__DATE__,3);

	if(strcmp(temp_mon,"Jan")==0)
	{
		memcpy(&comp_date[4],"01",2);
	}
	if(strcmp(temp_mon,"Feb")==0)
	{
		memcpy(&comp_date[4],"02",2);
	}
	if(strcmp(temp_mon,"Mar")==0)
	{
		memcpy(&comp_date[4],"03",2);
	}
	if(strcmp(temp_mon,"Apr")==0)
	{
		memcpy(&comp_date[4],"04",2);
	}
	if(strcmp(temp_mon,"May")==0)
	{
		memcpy(&comp_date[4],"05",2);
	}
	if(strcmp(temp_mon,"Jun")==0)
	{
		memcpy(&comp_date[4],"06",2);
	}
	if(strcmp(temp_mon,"Jul")==0)
	{
		memcpy(&comp_date[4],"07",2);
	}
	if(strcmp(temp_mon,"Aug")==0)
	{
		memcpy(&comp_date[4],"08",2);
	}
	if(strcmp(temp_mon,"Sep")==0)
	{
		memcpy(&comp_date[4],"09",2);
	}
	if(strcmp(temp_mon,"Oct")==0)
	{
		memcpy(&comp_date[4],"10",2);
	}
	if(strcmp(temp_mon,"Nov")==0)
	{
		memcpy(&comp_date[4],"11",2);
	}
	if(strcmp(temp_mon,"Dec")==0)
	{
		memcpy(&comp_date[4],"12",2);
	}

	memcpy(&comp_date[0],__DATE__+7,4);
	memcpy(&comp_date[6],__DATE__+4,2);
	if(comp_date[6]==' ')
	{
		comp_date[6]='0';
	}

//	strcpy(APP_VERSION,comp_date);
	strcpy(APP_VERSION,"20160429");

	FirmWare_Version = strtoul(APP_VERSION, NULL, 0);

	printf("\n\n\n============== APP_VERSION %s ========================\n\n\n",APP_VERSION);
}


/**********************************************
	use signal sem function
***********************************************/
sem_t  ptz_sem; //define
sem_init(&ptz_sem,0, 0); //init
sem_post(&ptz_sem); //post
//in thread to get signal and do something
while(gRunning)
{
    sem_wait(&ptz_sem);
}
sem_destroy(&ptz_sem); //destroy...

/******************************************************
	判断某文文件是否存在
******************************************************/
if(access("/mnt/mtd/system.cfg", F_OK) == 0)
{
	printf("====/mnt/mtd/system.cfg is exist =====\n"); 		   
}
else
{
	ret = check_and_create_system_cfg("/mnt/mtd/system.cfg");
	if(ret < 0)
	{
		printf("check and create system file fail \n");
		return -1;
	}
}


/*******************************************************
	检查某个网卡是否正常工作
********************************************************/
int CheckHaveNetCable(char * pNetAdapter)
{
	struct ifreq ifr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if( skfd < 0 )
	{
		DPRINTK("Open socket err\n");
		return -1;
	}

	strcpy(ifr.ifr_name, pNetAdapter);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		DPRINTK("SIOCGIFFLAGS err\n");
		close(skfd);
		return -1;
	}
	
   close(skfd);

   
	if(ifr.ifr_flags & IFF_RUNNING)
		return 1; 
	else
	return -1;
}

int ret =0;
ret =  CheckHaveNetCable("eth0");	

if(ret > 0)
{	   
	//printf(" ======line network is connect !======\n");
} else {
	//printf(" ======line network is disconnect !======\n");
}


/************************************************
		获取文件占用存储空间大小
************************************************/
FILE * pfile = NULL;
UINT nSize = 0;
if(NULL == (pfile = fopen("/mnt/mtd/system.cfg" , "rb")))
{
	return FALSE;
}
/////
fseek(pfile, 0L, SEEK_END);   
nSize = ftell(pfile);  
printf("nSize=%d , size=%d " , nSize ,sizeof(IPCAM_SYSCONFIG));


/****************************************************************************
	get and set  ip, mask addr, gateway ,MAC addr ,broadcast addr, DNS
	参数: eth0
****************************************************************************/

int get_ip_addr(char *name,char *net_ip)
{
	struct ifreq ifr;
	int ret = 0;
	int fd; 
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) 
	{
		ret = -1;
	}
	
	strcpy(net_ip,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

int get_mask_addr(char *name,char *net_mask)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;	
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0) 
	{
		ret = -1;
	}
	
	strcpy(net_mask,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

int get_brdcast_addr(char *name,char *net_brdaddr)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;	
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0) 
	{
		ret = -1;
	}
	
	strcpy(net_brdaddr,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}


int get_net_phyaddr(char *name, char *addr)  // name : eth0 eth1 lo and so on 
{											// addr : 12:13:14:15:16:17  mac addr
	int 		sockfd;		
	struct 	ifreq ifr;
	char 	buff[24];
	
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{		
		return -1;
	}

	/* get net physical address */
	strcpy(ifr.ifr_name, name);
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
	{		
		return -1;
	}
	strcpy(ifr.ifr_name, name);
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0)
	{
		return -1;	
	}
	else
	{
		memcpy(buff, ifr.ifr_hwaddr.sa_data, 8);
		snprintf(addr, sizeof(buff), "%02X:%02X:%02X:%02X:%02X:%02X",
			 (buff[0] & 0377), (buff[1] & 0377), (buff[2] & 0377),
			 (buff[3] & 0377), (buff[4] & 0377), (buff[5] & 0377));		
		printf("HW address: %s\n", addr);		
		return  0;
	}
}

int set_ip_addr(char *name,char *net_ip) /* name : eth0 eth1 lo and so on */
{	
	struct sockaddr	addr;
	struct ifreq ifr;
	char gateway_addr[32];
	int ret = 0;
	int fd;	
	
	((struct sockaddr_in *)&(addr))->sin_family = PF_INET;
	((struct sockaddr_in *)&(addr))->sin_addr.s_addr = inet_addr(net_ip);

	ifr.ifr_addr = addr;
	strcpy(ifr.ifr_name,name);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	get_gateway_addr(gateway_addr); /* 脭脷脡猫脰脙IP脢卤禄谩脟氓鲁媒脥酶脗莽,脣霉脪脭脧脠卤拢沤忙潞脙脥酶脗莽脡猫脰脙 */

	if (ioctl(fd, SIOCSIFADDR, &ifr) != 0) 
	{
		ret = -1;
	}

	close(fd);

	set_gateway_addr(gateway_addr); /*禄脰啪沤脥酶脗莽碌脛脥酶鹿脴脡猫脰脙 */

	return	ret;
}

int set_mask_addr(char *name,char *mask_ip) /* name : eth0 eth1 lo and so on */
{	
	struct sockaddr	addr;
	struct ifreq ifr;
	char gateway_addr[32];
	int ret = 0;
	int fd;	
	
	((struct sockaddr_in *)&(addr))->sin_family = PF_INET;
	((struct sockaddr_in *)&(addr))->sin_addr.s_addr = inet_addr(mask_ip);
	ifr.ifr_netmask = addr;
	strcpy(ifr.ifr_name,name);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	get_gateway_addr(gateway_addr); /* 脭脷脡猫脰脙IP脢卤禄谩脟氓鲁媒脥酶脗莽,脣霉脪脭脧脠卤拢沤忙潞脙脥酶脗莽脡猫脰脙 */

	if (ioctl(fd, SIOCSIFNETMASK, &ifr) != 0) 
	{
		ret = -1;
	}

	close(fd);

	set_gateway_addr(gateway_addr); /*禄脰啪沤脥酶脗莽碌脛脥酶鹿脴脡猫脰脙 */

	return	ret;
}

int set_gateway_addr(char *gateway_addr)
{
	char old_gateway_addr[32];
	struct rtentry rt;
	unsigned long gw;
	int fd;
	int ret = 0;
	
	get_gateway_addr(old_gateway_addr);
	del_gateway_addr(old_gateway_addr);
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		ret = -1;
	}	

	gw = inet_addr(gateway_addr);
	memset((char *) &rt, 0, sizeof(struct rtentry));

	((struct sockaddr_in *)&(rt.rt_dst))->sin_addr.s_addr = 0;
	
	rt.rt_flags = RTF_UP | RTF_GATEWAY ;
	//rt.rt_flags = 0x03;
	
	((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;
	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;
	
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	rt.rt_dev = NULL;
		
	if (ioctl(fd, SIOCADDRT, &rt) < 0) 	
	{	
		ret = -1;
	}
	close(fd);
	printf("[set_gateway_addr] gateway_addr=%s \n" , gateway_addr);
	return	ret;
}

int del_gateway_addr(char *gateway_addr)
{
	struct rtentry rt;
	unsigned long gw;
	int ret = 0;
	int fd;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
	
	gw = inet_addr(gateway_addr);
	memset((char *) &rt, 0, sizeof(struct rtentry));

	rt.rt_flags = RTF_UP | RTF_GATEWAY ;
	
	((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;
	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;
	
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	
	rt.rt_dev = NULL;
		
	if (ioctl(fd, SIOCDELRT, &rt) < 0) 	
	{
		ret = -1;
	}
	
	close(fd);

	return	ret;
}

/*****************************************************
					重启网口
*****************************************************/
int ifconfig_up_down(char *name, char *action) // name :  eth0 eth1 lo and so on 
{											   // action: down up	
	const struct options *op;
	int 	sockfd; 	
	int 	selector;
	struct	ifreq ifr;
	unsigned char mask;
	
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		return -1;
	}

	/* get interface name */
	safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);
	mask = N_MASK;
	
	for (op = OptArray ; op->name ; op++) 
	{		
			if (strcmp(action,op->name) == 0) 
			{
				if ((mask &= op->flags)) 
				{ 
					goto IFCONFIG_UP_DOWN_FOUND_ARG;
				}
			}
	}
	return -4;
		
IFCONFIG_UP_DOWN_FOUND_ARG: 	
	
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) 
	{
		return -2;
	} 
	else 
	{
		selector = op->selector;
		if (mask & SET_MASK) 
		{
			ifr.ifr_flags |= selector;
		} 
		else 
		{
			ifr.ifr_flags &= ~selector;
		}
		if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) 
		{
			return -3;
		}
	}						

	return	0;
}

int set_net_dns(char* pstrDns1 , char* pstrDns2)
{
	FILE		*pf;
	char		ipbuf[16];
	if(NULL == pstrDns1 || NULL == pstrDns2)
	{
		return -1;
	}
	if(0 > check_ip (pstrDns1) &&
		0 > check_ip (pstrDns2))
	{
		return -1;
	}	
	if(NULL == (pf=fopen("/tmp/resolv.conf","w")))
	{
		return -1;
	}	
	if(NULL != pstrDns1)
	{
		memset(ipbuf , 0 , sizeof(ipbuf));
		memcpy(ipbuf, pstrDns1, sizeof(ipbuf));
		fprintf(pf, "nameserver %s\n", ipbuf);
	}
	if(NULL != pstrDns2)
	{
		memset(ipbuf , 0 , sizeof(ipbuf));
		memcpy(ipbuf, pstrDns2, sizeof(ipbuf));
		fprintf(pf, "nameserver %s\n", ipbuf);
	}

	fclose(pf);	
	return 0;
}

int get_net_dns(char* pstrDns1 , char* pstrDns2)
{
	FILE 	*fp 		= NULL;
	char 	*buf		= NULL;	
	char 	name[80]	= {"\0"};
	char	strDns[80];
	int		bDns1 = 0;
	int 	bDns2 = 0;
	int 	len			= 0;
	int 	read		= 0;
	int 	iRet		= 0;
	
	if(NULL == pstrDns1 || NULL == pstrDns2 )
	{
		return -1;
	}
	if (NULL == (fp=fopen("/etc/resolv.conf", "r")))
	{
		return -1;
	}
	while((read = getline(&buf, &len, fp)) > 0)//读取第一个域名
	{  
		memset(strDns , 0 , sizeof(strDns));
		iRet = sscanf(buf, "%s %s", name, strDns);
		if (iRet != 2)
		{
			continue;
		}
		if ( !strncasecmp(name, "nameserver", strlen("nameserver")))
		{
			sprintf(pstrDns1 , "%s" , strDns);
			bDns1 = 1;
			break;
		}			
	 }

	while((read=getline(&buf,&len,fp)) > 0) //读取第二个域名
	{
		memset(strDns , 0 , sizeof(strDns));
		iRet = sscanf(buf, "%s %s", name, strDns);
		if (iRet != 2)
		{
			continue;
		}
		if ( !strncasecmp(name, "nameserver", strlen("nameserver")))
		{
			sprintf(pstrDns2 , "%s" , strDns);
			bDns2 = 1;			
			break;
		}
	}	
	fclose(fp);
	if(1 == bDns1 || 1 == bDns2)
	{
		return 0;
	}
	return -1;	
}


/************************************************************
	判断是否为一个点分十进制格式的ip地址
*************************************************************/
int inet_pton(int af,const char *src,void *dst)
{
	switch (af) {
	case AF_INET:
		return (inet_pton4(src, dst));
#ifdef INET_IPV6
	case AF_INET6:
		return (inet_pton6(src, dst));
#endif
	default:
		//__set_errno (EAFNOSUPPORT);
		return (-1);
	}
	/* NOTREACHED */
}

int check_ip (char *ip_str)
{
	struct sockaddr_in sa;
	if(ip_str==NULL)
	{
		return -1;
	}
	int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));
	if (result == 0)
	{ 
		return -1;
	}
	return 0;
}


/***************************************************************
	Linux thread start/close method(to start thread in thread function)
***************************************************************/
void *McuRF_Thread(int *param)
{
	//...
	pthread_detach(pthread_self());  //start this thread

	while(true)
	{
		//...
	}

	pthread_exit((void*)pthread_self());  //close thread
}


/******************************************************************
	异步select 机制的使用
******************************************************************/
fd_set rfds;
struct timeval tv ;
FD_ZERO(&rfds);// 清空串口接收端口集
FD_SET(nfd,&rfds);// 设置串口接收端口集
tv.tv_sec = 1;
tv.tv_usec = 0;
while(FD_ISSET(nfd,&rfds)) // 检测串口是否有读写动作
{
	if(select(nfd+1,&rfds,NULL,NULL,&tv)<0)
      	{
      		perror("select()");
		printf("select error!\n");
		return FALSE;
       }  
	else 
	{
		//...
	}
}


/******************************************************************
					给线程重命名
******************************************************************/
int IPCAM_setTskName(char * name)
{
	return prctl(PR_SET_NAME, name);
}


/******************************************************************
						线程函数宏
******************************************************************/
#define	IPCAM_PTHREAD_DETACH			do{ pthread_detach(pthread_self()); }while(0)
#define	IPCAM_PTHREAD_EXIT			do{ pthread_exit((void*)pthread_self()); }while(0)
	
	
#define	IPCAM_CREATE_THREAD(Func, Args)	do{					\
			pthread_t		__pth__;									\
			if(pthread_create(&__pth__, NULL, (void *)Func, (void *)Args))	\
			return FALSE; \
		  }while(0)
		
#define	IPCAM_CREATE_THREADEX(Func, Args,Ret)	do{					\
			pthread_t		__pth__;									\
			if (0 == pthread_create(&__pth__, NULL, (void *)Func, (void *)Args))	\
				Ret = TRUE; \
			else \
				Ret = FALSE; \
		  }while(0)

//useage
INT iRet		= 0;
IPCAM_CREATE_THREADEX(Thread_UploadJpg, NULL, iRet);

if (TRUE != iRet){
	return FALSE;
}
else 
{
	return TRUE;
}

static VOID* Thread_UploadJpg(VOID* Argu)
{
	
}


/********************************************************************
					AES encode and decode function
					file:AES_EnDe.h AES_EnDe.c
********************************************************************/
strcpy(gDevInfo.sDevPwd, system_param.p2p_access_ps);
strcpy(gDevInfo.sAESKey, "Greate P2P!!!!!");

AES_Init();
AES_Encrypt(128,(unsigned char *)gDevInfo.sDevPwd, strlen(gDevInfo.sDevPwd), (unsigned char *)gDevInfo.sAESKey, 16, (unsigned char * )gDevInfo.sDevPwdAES);
AES_Encrypt(128,(unsigned char *)UPGRADE_PWD, strlen(UPGRADE_PWD), (unsigned char *)gDevInfo.sAESKey, 16, (unsigned char * )update_password);
AES_Deinit();

AES_Decrypt(128, (UCHAR *)oldDevPasswd_enc, 16, (unsigned char *)gDevInfo.sAESKey, 16, (UCHAR * )set_pwd_req.oldDevPasswd);


