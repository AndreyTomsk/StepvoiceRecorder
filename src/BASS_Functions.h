
#ifndef _BASS_FUNCTIONS_H
#define _BASS_FUNCTIONS_H

#include <bass.h>

namespace Bass
{
	float GetMaxPeakDB(HRECORD a_handle);

	void CALLBACK LoopbackStreamDSP(HDSP handle, DWORD channel, void *buffer,
		DWORD length, void *user);

	void CALLBACK StreamMuteDSP(HDSP handle, DWORD channel, void *buffer,
		DWORD length, void *user);

} // namespace Bass

#endif //_BASS_FUNCTIONS_H
