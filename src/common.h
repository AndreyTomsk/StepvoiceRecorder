#ifndef _COMMON_H_
#define _COMMON_H_

#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#define SAFE_DELETE_ARRAY(p) if(p) { delete [] p; p = NULL; }

#define CLEAR_STRUCT(pStruct) if(pStruct != NULL) { ZeroMemory((pStruct), sizeof(*pStruct)); }

//WASAPI COM wrappers related macro (see WasapiHelpers.cpp)
#define EIF(x) if (FAILED(hr=(x))) { goto Exit; }	// Exit If Failed.
#define SAFE_RELEASE(x) { if (x != NULL) { x.Release(); x = NULL; } }

#endif