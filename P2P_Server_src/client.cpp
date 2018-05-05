#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>   

#ifndef WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#include <iostream>
//#include <udt.h>
//#include "udp.h"
//#include "stun.h"
#include "FTCDebug.h"
//#include "cc.h"
//#include "test_util.h"


using namespace std;


void
usage()
{
   cerr << "Usage:" << endl
	<< "    ./client stunServerHostname [testNumber] [-v] [-p srcPort] [-i nicAddr1] [-i nicAddr2] [-i nicAddr3]" << endl
	<< "For example, if the STUN server was larry.gloo.net, you could do:" << endl
	<< "    ./client larry.gloo.net" << endl
	<< "The testNumber is just used for special tests." << endl
	<< " test 1 runs test 1 from the RFC. For example:" << endl
	<< "    ./client larry.gloo.net 0" << endl << endl
        << endl;
}

#define MAX_NIC 3
/*
void getNATtype(int argc, char* argv[])
{
	initNetwork();
    
   cout << "STUN client version " << STUN_VERSION << endl;
   
   int testNum = 0;
   bool verbose = true; //false;
	
   StunAddress4 stunServerAddr;
   stunServerAddr.addr=0;

   int srcPort=0;
   StunAddress4 sAddr[MAX_NIC];
   int retval[MAX_NIC];
   int numNic=0;

   for ( int i=0; i<MAX_NIC; i++ )
   {
      sAddr[i].addr=0; 
      sAddr[i].port=0; 
      retval[i]=0;
   }
   
   for ( int arg = 1; arg<argc; arg++ )
   {
      if ( !strcmp( argv[arg] , "-v" ) )
      {
         verbose = true;
      }
      else if ( !strcmp( argv[arg] , "-i" ) )
      {
         arg++;
         if ( argc <= arg ) 
         {
            usage();
            exit(-1);
         }
         if ( numNic >= MAX_NIC )
         {  
            cerr << "Can not have more than "<<  MAX_NIC <<" -i options" << endl;
            usage();
            exit(-1);
         }
         
         stunParseServerName(argv[arg], sAddr[numNic++]);
      }
      else if ( !strcmp( argv[arg] , "-p" ) )
      {
         arg++;
         if ( argc <= arg ) 
         {
            usage();
            exit(-1);
         }
         srcPort = strtol( argv[arg], NULL, 10);
      }
      else    
      {
        char* ptr;
        int t =  strtol( argv[arg], &ptr, 10 );
        if ( *ptr == 0 )
        { 
           // conversion worked
           testNum = t;
           cout << "running test number " << testNum  << endl; 
        }
        else
        {
           bool ret = stunParseServerName( argv[arg], stunServerAddr);
           if ( ret != true )
           {
              cerr << argv[arg] << " is not a valid host name " << endl;
              usage();
              exit(-1);
           }
	}	
      }
   }

   if ( srcPort == 0 )
   {
      srcPort = stunRandomPort();
   }
   
   if ( numNic == 0 )
   {
      // use default 
      numNic = 1;
   }
   
   for ( int nic=0; nic<numNic; nic++ )
   {
      sAddr[nic].port=srcPort;
      if ( stunServerAddr.addr == 0 )
      {
         usage();
         exit(-1);
      }
   
      if (testNum==0)
      {
         bool presPort=false;
         bool hairpin=false;
		
         NatType stype = stunNatType( stunServerAddr, verbose, &presPort, &hairpin, 
                                      srcPort, &sAddr[nic]);
		
         if ( nic == 0 )
         {
            cout << "Primary: ";
         }
         else
         {
            cout << "Secondary: ";
         }
         
         switch (stype)
         {
            case StunTypeFailure:
               cout << "Some stun error detetecting NAT type";
	       retval[nic] = -1;
               exit(-1);
               break;
            case StunTypeUnknown:
               cout << "Some unknown type error detetecting NAT type";
	       retval[nic] = 0xEE;
               break;
            case StunTypeOpen:
               cout << "Open";
	       retval[nic] = 0x00; 
               break;
            case StunTypeIndependentFilter:
               cout << "Independent Mapping, Independent Filter";
               if ( presPort ) cout << ", preserves ports"; else cout << ", random port";
               if ( hairpin  ) cout << ", will hairpin"; else cout << ", no hairpin";
               retval[nic] = 0x02;
               break;
            case StunTypeDependentFilter:
               cout << "Independent Mapping, Address Dependent Filter";
               if ( presPort ) cout << ", preserves ports"; else cout << ", random port";
               if ( hairpin  ) cout << ", will hairpin"; else cout << ", no hairpin";
               retval[nic] = 0x04;
               break;
            case StunTypePortDependedFilter:
               cout << "Independent Mapping, Port Dependent Filter";
               if ( presPort ) cout << ", preserves ports"; else cout << ", random port";
               if ( hairpin  ) cout << ", will hairpin"; else cout << ", no hairpin";
               retval[nic] = 0x06;
               break;
            case StunTypeDependentMapping:
               cout << "Dependent Mapping";
               if ( presPort ) cout << ", preserves ports"; else cout << ", random port";
               if ( hairpin  ) cout << ", will hairpin"; else cout << ", no hairpin";
               retval[nic] = 0x08;
               break;
            case StunTypeFirewall:
               cout << "Firewall";
               retval[nic] = 0x0A;
               break;
            case StunTypeBlocked:
               cout << "Blocked or could not reach STUN server";
               retval[nic] = 0x0C;
               break;
            default:
               cout << stype;
               cout << "Unkown NAT type";
               retval[nic] = 0x0E;  // Unknown NAT type
               break;
         }
         cout << "\t"; cout.flush();
         
         if (!hairpin)
         {
             retval[nic] |= 0x10;
         }       

         if (presPort)
         {
             retval[nic] |= 0x01;
         }
      }
      else if (testNum==100)
      {
         Socket myFd = openPort(srcPort,sAddr[nic].addr,verbose);
      
         StunMessage req;
         memset(&req, 0, sizeof(StunMessage));
      
         StunAtrString username;
         StunAtrString password;
         username.sizeValue = 0;
         password.sizeValue = 0;
      
         stunBuildReqSimple( &req, username, 
                             false , false , 
                             0x0c );
      
         char buf[STUN_MAX_MESSAGE_SIZE];
         int len = STUN_MAX_MESSAGE_SIZE;
      
         len = stunEncodeMessage( req, buf, len, password,verbose );
      
         if ( verbose )
         {
            cout << "About to send msg of len " << len 
                 << " to " << stunServerAddr << endl;
         }
      
         while (1)
         {
            for ( int i=0; i<100; i++ )
            {
               sendMessage( myFd,
                            buf, len, 
                            stunServerAddr.addr, 
                            stunServerAddr.port,verbose );
            }
#ifdef WIN32 // !cj! TODO - should fix this up in windows
            clock_t now = clock();
            assert( CLOCKS_PER_SEC == 1000 );
            while ( clock() <= now+10 ) { };
#else
            usleep(10*1000);
#endif
         }
      }
      else if (testNum==-2)
      {
         const int numPort = 5;
         int fd[numPort];
         StunAddress4 mappedAddr;
         
         for( int i=0; i<numPort; i++ )
         {
            fd[i] = stunOpenSocket( stunServerAddr, &mappedAddr,
                                    (srcPort==0)?0:(srcPort+i), &sAddr[nic],
                                    verbose );
            cout << "Got port at " << mappedAddr.port << endl;
         }
          
         for( int i=0; i<numPort; i++ )
         {
            closesocket(fd[i]);
         }
      }
      else if (testNum==-1)
      {
         int fd3,fd4;
         StunAddress4 mappedAddr;
         
         bool ok = stunOpenSocketPair(stunServerAddr,
                                      &mappedAddr,
                                      &fd3,
                                      &fd4,
                                      srcPort, 
                                      &sAddr[nic],
                                      verbose);
         if ( ok )
         {
            closesocket(fd3);
            closesocket(fd4);
            cout << "Got port pair at " << mappedAddr.port << endl;
         }
         else
         {
            cerr << "Opened a stun socket pair FAILED" << endl;
         }
      }
      else
      {
         stunTest( stunServerAddr,testNum,verbose,&(sAddr[nic]) );
      }
   } // end of for loop 
   cout << endl;
   
   UInt32 ret=0;
   for ( int i=numNic-1; i>=0; i-- )
   {
      if ( retval[i] == -1 )
      {
         ret = 0xFFFFFFFF;
         break;
      }
      ret = ret << 8;
      ret = ret | ( retval[i] & 0xFF );
   }
   
   cout << "Return value is " << hex << "0x";
   cout.fill('0');
   cout.width(6);
   cout << ret << dec << endl;
   cout.fill(' ');
}
*/ //hly2070
/*
#ifndef WIN32
void* monitor(void* s)
#else
DWORD WINAPI monitor(LPVOID s)
#endif
{
   UDTSOCKET u = *(UDTSOCKET*)s;

   UDT::TRACEINFO perf;

   cout << "SendRate(Mb/s)\tRTT(ms)\tCWnd\tPktSndPeriod(us)\tRecvACK\tRecvNAK" << endl;

   while (true)
   {
      #ifndef WIN32
         sleep(1);
      #else
         Sleep(1000);
      #endif

      if (UDT::ERROR == UDT::perfmon(u, &perf))
      {
         cout << "perfmon: " << UDT::getlasterror().getErrorMessage() << endl;
         break;
      }

      cout << perf.mbpsSendRate << "\t\t" 
           << perf.msRTT << "\t" 
           << perf.pktCongestionWindow << "\t" 
           << perf.usPktSndPeriod << "\t\t\t" 
           << perf.pktRecvACK << "\t" 
           << perf.pktRecvNAK << endl;
   }

   #ifndef WIN32
      return NULL;
   #else
      return 0;
   #endif
}
*/
int main(int argc, char* argv[])
{
#if 0 
   assert( sizeof(UInt8 ) == 1 );
   assert( sizeof(UInt16) == 2 );
   assert( sizeof(UInt32) == 4 );

   //STUN client
   getNATtype(argc, argv);

   //UDT client
   // Automatically start up and clean up UDT module.
   UDTUpDown _udt_;

   struct addrinfo hints, *local, *peer;

   memset(&hints, 0, sizeof(struct addrinfo));

   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   //hints.ai_socktype = SOCK_DGRAM;

   if (0 != getaddrinfo(NULL, "9000", &hints, &local))
   {
      cout << "incorrect network address.\n" << endl;
      return 0;
   }

   UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);

   // UDT Options
   //UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
   //UDT::setsockopt(client, 0, UDT_MSS, new int(9000), sizeof(int));
   //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10000000), sizeof(int));
   //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(10000000), sizeof(int));
   //UDT::setsockopt(client, 0, UDT_MAXBW, new int64_t(12500000), sizeof(int));

   // Windows UDP issue
   // For better performance, modify HKLM\System\CurrentControlSet\Services\Afd\Parameters\FastSendDatagramThreshold
   #ifdef WIN32
      UDT::setsockopt(client, 0, UDT_MSS, new int(1052), sizeof(int));
   #endif

   // for rendezvous connection, enable the code below
   /*
   UDT::setsockopt(client, 0, UDT_RENDEZVOUS, new bool(true), sizeof(bool));
   if (UDT::ERROR == UDT::bind(client, local->ai_addr, local->ai_addrlen))
   {
      cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }
   */

   freeaddrinfo(local);

   string service("9000");

   if (0 != getaddrinfo(argv[1], service.c_str(), &hints, &peer))
   {
      cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
      return 0;
   }

   // connect to the server, implict bind
   if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
   {
      cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   freeaddrinfo(peer);

   // using CC method
   //CUDPBlast* cchandle = NULL;
   //int temp;
   //UDT::getsockopt(client, 0, UDT_CC, &cchandle, &temp);
   //if (NULL != cchandle)
   //   cchandle->setRate(500);

   int size = 100000;
   char* data = new char[size];

   #ifndef WIN32
      pthread_create(new pthread_t, NULL, monitor, &client);
   #else
      CreateThread(NULL, 0, monitor, &client, 0, NULL);
   #endif

   for (int i = 0; i < 1000000; i ++)
   {
      int ssize = 0;
      int ss;
      while (ssize < size)
      {
         if (UDT::ERROR == (ss = UDT::send(client, data + ssize, size - ssize, 0)))
         {
            cout << "send:" << UDT::getlasterror().getErrorMessage() << endl;
            break;
         }

         ssize += ss;
      }

      if (ssize < size)
         break;
   }

   UDT::close(client);
   delete [] data;
#endif    
   return 0;
}


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

// Local Variables:
// mode:c++
// c-file-style:"ellemtel"
// c-file-offsets:((case-label . +))
// indent-tabs-mode:nil
// End:
