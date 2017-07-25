

#include <stdio.h>
#include <string.h>
#include "AES_EnDe.h"


#define AS_LIB
#define  PUB_Linux

#ifdef PUB_Win32
	#include <Windows.h>
	#define pub_msecSleep(ms)	Sleep(ms)

	typedef CRITICAL_SECTION			aes_mutex_t;
	#define aes_mutex_init(theMutex)	InitializeCriticalSection(&theMutex)
	#define aes_mutex_lock(theMutex)	EnterCriticalSection(&theMutex)
	#define aes_mutex_unlock(theMutex)	LeaveCriticalSection(&theMutex)
	#define aes_mutex_destroy(theMutex)	DeleteCriticalSection(&theMutex)
#endif

#ifdef PUB_Linux
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <signal.h>
	#include <time.h>
	#include <sys/time.h>
	#include <stdarg.h>
	#include <dirent.h>
	#include <semaphore.h>
	#include <ctype.h>

	#define pub_msecSleep(ms)	usleep(ms * 1000)

	typedef pthread_mutex_t				aes_mutex_t;
	#define aes_mutex_init(theMutex)	pthread_mutex_init(&theMutex, NULL)
	#define aes_mutex_lock(theMutex)	pthread_mutex_lock(&theMutex)
	#define aes_mutex_unlock(theMutex)	pthread_mutex_unlock(&theMutex)
	#define aes_mutex_destroy(theMutex)	
#endif

//======================================================================================================
// The number of columns comprising a g_state g_in AES. 
// This is a constant g_in AES. Value=4
#define Nb 4

static char g_bAESMutexInit=0;
aes_mutex_t g_mutexAESHandle;

// The number of rounds g_in AES Cipher. It is simply initiated to zero. 
// The actual value is recieved g_in the program.
int g_Nr=0;

// The number of 32 bit words g_in the key. It is simply initiated to zero. 
// The actual value is recieved g_in the program.
int g_Nk=0;

// g_in - it is the array that holds the plain text to be encrypted.
// g_out - it is the array that holds the key for encryption.
// g_state - the array that holds the intermediate results during encryption.
unsigned char g_in[16]={0}, g_out[16]={0}, g_state[4][4];

// The array that stores the round keys.
unsigned char g_RoundKey[240];

// The g_Key input to the AES Program
unsigned char g_Key[32];


int getSBoxValue(int num)
{
	int sbox[256] =   {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
	return sbox[num];
}

// The round constant word array, Rcon[i], contains the values given by 
// x to th e power (i-1) being powers of x (x is denoted as {02}) g_in the field GF(28)
// Note that i starts at 1, not 0).
int Rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };

// This function produces Nb(g_Nr+1) round keys. The round keys are used g_in each round to encrypt the states. 
void KeyExpansion()
{
	int i,j;
	unsigned char temp[4],k;
	
	// The first round key is the key itself.
	for(i=0;i<g_Nk;i++)
	{
		g_RoundKey[i*4]=g_Key[i*4];
		g_RoundKey[i*4+1]=g_Key[i*4+1];
		g_RoundKey[i*4+2]=g_Key[i*4+2];
		g_RoundKey[i*4+3]=g_Key[i*4+3];
	}

	// All other round keys are found from the previous round keys.
	while (i < (Nb * (g_Nr+1)))
	{
		for(j=0;j<4;j++) temp[j]=g_RoundKey[(i-1) * 4 + j];
		if (i % g_Nk == 0)
		{
			// This function rotates the 4 bytes g_in a word to the left once.
			// [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

			// Function RotWord()
			{
				k = temp[0];
				temp[0] = temp[1];
				temp[1] = temp[2];
				temp[2] = temp[3];
				temp[3] = k;
			}

			// SubWord() is a function that takes a four-byte input word and 
			// applies the S-box to each of the four bytes to produce an output word.

			// Function Subword()
			{
				temp[0]=getSBoxValue(temp[0]);
				temp[1]=getSBoxValue(temp[1]);
				temp[2]=getSBoxValue(temp[2]);
				temp[3]=getSBoxValue(temp[3]);
			}

			temp[0] =  temp[0] ^ Rcon[i/g_Nk];

		}else if (g_Nk > 6 && i % g_Nk == 4){
			// Function Subword()
			{
				temp[0]=getSBoxValue(temp[0]);
				temp[1]=getSBoxValue(temp[1]);
				temp[2]=getSBoxValue(temp[2]);
				temp[3]=getSBoxValue(temp[3]);
			}
		}
		g_RoundKey[i*4+0] = g_RoundKey[(i-g_Nk)*4+0] ^ temp[0];
		g_RoundKey[i*4+1] = g_RoundKey[(i-g_Nk)*4+1] ^ temp[1];
		g_RoundKey[i*4+2] = g_RoundKey[(i-g_Nk)*4+2] ^ temp[2];
		g_RoundKey[i*4+3] = g_RoundKey[(i-g_Nk)*4+3] ^ temp[3];
		i++;
	}
}

// This function adds the round key to g_state.
// The round key is added to the g_state by an XOR function.
void AddRoundKey(int round) 
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_state[j][i] ^= g_RoundKey[round * Nb * 4 + i * Nb + j];
	}
}

// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

//=={{AES_Encrypt=======================================================================================
// The SubBytes Function Substitutes the values g_in the
// g_state matrix with values g_in an S-box.
void SubBytes()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_state[i][j] = getSBoxValue(g_state[i][j]);
	}
}

// The ShiftRows() function shifts the rows g_in the g_state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void ShiftRows()
{
	unsigned char temp;

	// Rotate first row 1 columns to left	
	temp=g_state[1][0];
	g_state[1][0]=g_state[1][1];
	g_state[1][1]=g_state[1][2];
	g_state[1][2]=g_state[1][3];
	g_state[1][3]=temp;

	// Rotate second row 2 columns to left	
	temp=g_state[2][0];
	g_state[2][0]=g_state[2][2];
	g_state[2][2]=temp;

	temp=g_state[2][1];
	g_state[2][1]=g_state[2][3];
	g_state[2][3]=temp;

	// Rotate third row 3 columns to left
	temp=g_state[3][0];
	g_state[3][0]=g_state[3][3];
	g_state[3][3]=g_state[3][2];
	g_state[3][2]=g_state[3][1];
	g_state[3][1]=temp;
}

// MixColumns function mixes the columns of the g_state matrix
void MixColumns()
{
	int i;
	unsigned char Tmp,Tm,t;
	for(i=0;i<4;i++)
	{	
		t=g_state[0][i];
		Tmp = g_state[0][i] ^ g_state[1][i] ^ g_state[2][i] ^ g_state[3][i] ;
		Tm = g_state[0][i] ^ g_state[1][i] ; Tm = xtime(Tm); g_state[0][i] ^= Tm ^ Tmp ;
		Tm = g_state[1][i] ^ g_state[2][i] ; Tm = xtime(Tm); g_state[1][i] ^= Tm ^ Tmp ;
		Tm = g_state[2][i] ^ g_state[3][i] ; Tm = xtime(Tm); g_state[2][i] ^= Tm ^ Tmp ;
		Tm = g_state[3][i] ^ t ; Tm = xtime(Tm); g_state[3][i] ^= Tm ^ Tmp ;
	}
}

// Cipher is the main function that encrypts the PlainText.
void Cipher()
{
	int i,j,round=0;

	//Copy the input PlainText to g_state array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_state[j][i] = g_in[i*4 + j];
	}

	// Add the First round key to the g_state before starting the rounds.
	AddRoundKey(0); 
	
	// There will be g_Nr rounds.
	// The first g_Nr-1 rounds are identical.
	// These g_Nr-1 rounds are executed g_in the loop below.
	for(round=1;round<g_Nr;round++)
	{
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(round);
	}
	
	// The last round is given below.
	// The MixColumns function is not here g_in the last round.
	SubBytes();
	ShiftRows();
	AddRoundKey(g_Nr);

	// The encryption process is over.
	// Copy the g_state array to output array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_out[i*4+j]=g_state[j][i];
	}
}
//==}}AES_Encrypt=======================================================================================


//=={{AES_Decrypt=======================================================================================
int getSBoxInvert(int num)
{
	int rsbox[256] =
	{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb
	, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb
	, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e
	, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25
	, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92
	, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84
	, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06
	, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b
	, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73
	, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e
	, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b
	, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4
	, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f
	, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef
	, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61
	, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
	
	return rsbox[num];
}

// The SubBytes Function Substitutes the values g_in the
// g_state matrix with values g_in an S-box.
void InvSubBytes()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_state[i][j] = getSBoxInvert(g_state[i][j]);
	}
}

// The ShiftRows() function shifts the rows g_in the g_state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void InvShiftRows()
{
	unsigned char temp;
	
	// Rotate first row 1 columns to right	
	temp=g_state[1][3];
	g_state[1][3]=g_state[1][2];
	g_state[1][2]=g_state[1][1];
	g_state[1][1]=g_state[1][0];
	g_state[1][0]=temp;
	
	// Rotate second row 2 columns to right	
	temp=g_state[2][0];
	g_state[2][0]=g_state[2][2];
	g_state[2][2]=temp;
	
	temp=g_state[2][1];
	g_state[2][1]=g_state[2][3];
	g_state[2][3]=temp;
	
	// Rotate third row 3 columns to right
	temp=g_state[3][0];
	g_state[3][0]=g_state[3][1];
	g_state[3][1]=g_state[3][2];
	g_state[3][2]=g_state[3][3];
	g_state[3][3]=temp;
}

// Multiplty is a macro used to multiply numbers g_in the field GF(2^8)
#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))

// MixColumns function mixes the columns of the g_state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
void InvMixColumns()
{
	int i;
	unsigned char a,b,c,d;
	for(i=0;i<4;i++)
	{
		a = g_state[0][i];
		b = g_state[1][i];
		c = g_state[2][i];
		d = g_state[3][i];
		
		g_state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		g_state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		g_state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		g_state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
}

// InvCipher is the main function that decrypts the CipherText.
void InvCipher()
{
	int i,j,round=0;
	
	//Copy the input CipherText to g_state array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_state[j][i] = g_in[i*4 + j];
	}
	
	// Add the First round key to the g_state before starting the rounds.
	AddRoundKey(g_Nr); 
	
	// There will be g_Nr rounds.
	// The first g_Nr-1 rounds are identical.
	// These g_Nr-1 rounds are executed g_in the loop below.
	for(round=g_Nr-1;round>0;round--)
	{
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(round);
		InvMixColumns();
	}
	
	// The last round is given below.
	// The MixColumns function is not here g_in the last round.
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);
	
	// The decryption process is over.
	// Copy the g_state array to output array.
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++) g_out[i*4+j]=g_state[j][i];
	}
}
//==}}AES_Decrypt=======================================================================================

void AES_Init()
{
	if(g_bAESMutexInit) return;
	g_bAESMutexInit=1;
	aes_mutex_init(g_mutexAESHandle);
}

void AES_Deinit()
{
	return;

	if(g_bAESMutexInit) {
		aes_mutex_destroy(g_mutexAESHandle);
		g_bAESMutexInit=0;
	}
}

int AES_Encrypt(int nNumRounds, unsigned char *in_PlainText, int nSize_in,
				unsigned char *sKey, int nSize_skey, unsigned char *out_Encry)
{
	int i=0;

	if(nNumRounds!=128 && nNumRounds!=192 && nNumRounds!=256) return AES_ER_WRONG_NUM_ROUNDS;
	if(sKey==NULL || out_Encry==NULL|| nSize_in<0 || //in_PlainText==NULL || nSize_in<=0 || 
	   nSize_skey<0) return AES_ER_INVALID_PARAMETER;
	
	aes_mutex_lock(g_mutexAESHandle);
	g_Nr=nNumRounds;
	// Calculate g_Nk and g_Nr from the recieved value.	
	g_Nk = g_Nr / 32;	//4,  6,8
	g_Nr = g_Nk + 6;	//10,12,14
	
	{
		unsigned char tmpKey[32]={0};
		unsigned char tmpIn[32] ={0};
		int nSizeCpy=32;
		nSizeCpy=(nSize_in>32) ? 32 : nSize_in;
		memset(tmpIn, 0x0, sizeof(tmpIn));		//padding is 0x0
		memcpy(tmpIn, in_PlainText, nSizeCpy);

		nSizeCpy=32;
		nSizeCpy=(nSize_skey>32) ? 32 : nSize_skey;
		memset((void *)tmpKey, 0x0, sizeof(tmpKey)); //padding is 0x0
		memcpy((void *)tmpKey, (void *)sKey, nSizeCpy);

		// Copy the g_Key and PlainText
		for(i=0; i<g_Nk*4; i++)
		{
			g_Key[i]=tmpKey[i];
			g_in[i] =tmpIn[i];
		}
	#if defined(OUTPUT_DEBUG_MSG)
		{ //debug info
			char str[256]={"  GenA: "}, strTmp[128]={0};
			for(i=0; i<16; i++) {
				sprintf(strTmp, "%02X ", g_in[i]);
				strcat(str, strTmp);
			}
			strcat(str, "\n");

			strcat(str, "  Key(user pw): ");
			for(i=0; i<16; i++) {
				sprintf(strTmp, "%02X ", g_Key[i]);
				strcat(str, strTmp);
			}
			strcat(str, "\n");
		}
	#endif
	}

	// The KeyExpansion routine must be called before encryption.
	KeyExpansion();
	
	// The next function call encrypts the PlainText with the g_Key using AES algorithm.
	Cipher();

	memcpy((void *)out_Encry, (void *)g_out, 16);
	aes_mutex_unlock(g_mutexAESHandle);

	return 16;
}

int AES_Decrypt(int nNumRounds, unsigned char *in_EncryText, int nSize_in,
				unsigned char *sKey, int nSize_skey, unsigned char *out_Plain)
{
	int i=0;
	
	if(nNumRounds!=128 && nNumRounds!=192 && nNumRounds!=256) return AES_ER_WRONG_NUM_ROUNDS;
	if(sKey==NULL || out_Plain==NULL||  nSize_in<0 || //in_EncryText==NULL || 
	   nSize_skey<0) return AES_ER_INVALID_PARAMETER;

        aes_mutex_lock(g_mutexAESHandle);
	g_Nr=nNumRounds;
	// Calculate g_Nk and g_Nr from the recieved value.	
	g_Nk = g_Nr / 32;	//4,6,8
	g_Nr = g_Nk + 6;	//10,12,14

	{
		unsigned char tmpKey[32]={0};
		unsigned char tmpIn[32] ={0};
		int nSizeCpy=32;
		nSizeCpy=(nSize_in>32) ? 32 : nSize_in;
		memset(tmpIn, 0x0, sizeof(tmpIn));		//padding is 0x0
		memcpy(tmpIn, in_EncryText, nSizeCpy);
		
		nSizeCpy=32;
		nSizeCpy=(nSize_skey>32) ? 32 : nSize_skey;
		memset(tmpKey, 0x0, sizeof(tmpKey));	//padding is 0x0
		memcpy(tmpKey, sKey, nSizeCpy);
		
		// Copy the g_Key and PlainText
		for(i=0; i<g_Nk*4; i++)
		{
			g_Key[i]=tmpKey[i];
			g_in[i] =tmpIn[i];
		}
	}

	//The g_Key-Expansion routine must be called before the decryption routine.
	KeyExpansion();
	
	// The next function call decrypts the CipherText with the g_Key using AES algorithm.
	InvCipher();

	memcpy(out_Plain, g_out, 16);
	aes_mutex_unlock(g_mutexAESHandle);
	return 16;
}
