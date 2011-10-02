
#ifndef _BASS_FUNCTIONS_H
#define _BASS_FUNCTIONS_H

////////////////////////////////////////////////////////////////////////////////

namespace Bass
{
	float GetMaxPeakDB(DWORD a_handle);

	void CALLBACK LoopbackStreamDSP(DWORD handle, DWORD channel, void *buffer,
		DWORD length, void *user);

	void CALLBACK StreamMuteDSP(DWORD handle, DWORD channel, void *buffer,
		DWORD length, void *user);

	bool IsPlaybackDeviceValid(int a_device);
	bool IsRecordingDeviceValid(int a_device);

} // namespace Bass

////////////////////////////////////////////////////////////////////////////////

#endif //_BASS_FUNCTIONS_H
