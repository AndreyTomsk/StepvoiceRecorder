#ifndef _COMMON_H_
#define _COMMON_H_

#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#define SAFE_DELETE_ARRAY(p) if(p) { delete [] p; p = NULL; }

#define CLEAR_STRUCT(pStruct) if(pStruct) { ZeroMemory((pStruct), sizeof(*pStruct)); }

#endif