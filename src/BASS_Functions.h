
#ifndef _BASS_FUNCTIONS_H
#define _BASS_FUNCTIONS_H

#include <vector>

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


	// New features, helping with WASAPI recording
	// We need to correctly identify (selected) device, etc.

	typedef std::pair<DWORD, CString> DeviceIdNamePair;
	typedef std::vector<DeviceIdNamePair> DevicesArray;

	DevicesArray GetWasapiDevicesList();
	DeviceIdNamePair GetDefaultRecordingDevice();

} // namespace Bass

////////////////////////////////////////////////////////////////////////////////

#endif //_BASS_FUNCTIONS_H
