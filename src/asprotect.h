#ifndef ASPROTECT_H
#define ASPROTECT_H

#ifdef __BORLANDC__

 #define REG_CRYPT_BEGIN __emit__ (0xEB,0x04,0xEB,0x05,0x89,0x01);
 #define REG_CRYPT_END   __emit__ (0xEB,0x04,0xEB,0x05,0x99,0x01);

#else

 #define REG_CRYPT_BEGIN \
 __asm _emit 0xEB \
 __asm _emit 0x04 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x89 \
 __asm _emit 0x01		

 #define REG_CRYPT_END \
 __asm _emit 0xEB \
 __asm _emit 0x04 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x99 \
 __asm _emit 0x01 

#endif


#define APIConst		0xFFFFFFFF
#define capiDecrypt			 4
#define capiGetRegInfo			 5
#define capiGetTrialDays  		 8
#define capiGetTrialExecs 		 9

typedef DWORD (__stdcall  *TapiDecrypt) (IN char   *Key, IN DWORD  KeySize);
typedef char* (__stdcall  *TapiGetRegInfo) ();
typedef DWORD (__stdcall  *TapiGetTrialDays) ();
typedef DWORD (__stdcall  *TapiGetTrialExecs) ();

#endif