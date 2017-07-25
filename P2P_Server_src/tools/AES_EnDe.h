
#ifndef _AES_ENDE_H_
#define _AES_ENDE_H_

#define AES_No_ERROR				 0
#define AES_ER_WRONG_NUM_ROUNDS		-1 //nNumRounds=128,192,256
#define AES_ER_INVALID_PARAMETER	-2


#ifdef __cplusplus
	extern "C"{
#endif
		void AES_Init();
		void AES_Deinit();

	//NOTE: out_Encry must be >=32
	//		max size of nSize_in or nSize_skey is 32 when nNumRounds=128, 192, 256
	//		size outputing is 16, 24, 32
		int AES_Encrypt(int nNumRounds, unsigned char *in_PlainText, int nSize_in,
						unsigned char *sKey, int nSize_skey, unsigned char *out_Encry);

	//NOTE: out_Plain must be >=32
	//		max size of nSize_in or nSize_skey is 32 when nNumRounds=128, 192, 256
	//		size outputing is 16, 24, 32
		int AES_Decrypt(int nNumRounds, unsigned char *in_EncryText, int nSize_in,
						unsigned char *sKey, int nSize_skey, unsigned char *out_Plain);

#ifdef __cplusplus
	}
#endif

#endif