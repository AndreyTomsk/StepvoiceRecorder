#ifndef _WASAPI_HELPERS_H
#define _WASAPI_HELPERS_H
#pragma once

#include <vector>

/////////////////////////////////////////////////////////////////////////////

namespace WasapiHelpers
{
	// New features, helping with WASAPI recording
	// We need to correctly identify (selected) device, etc.

	typedef std::pair<DWORD, CString> DeviceIdNamePair;
	typedef std::vector<DeviceIdNamePair> DevicesArray;

	DevicesArray GetRecordingDevicesList();
	DeviceIdNamePair GetDefaultRecordingDevice(); //can be EMPTY_DEVICE

	extern const DeviceIdNamePair EMPTY_DEVICE;

	BOOL GetDeviceActualData(int device, DWORD freq, DWORD chans,
		DWORD& actualFreq, DWORD& actualChans);

	//We can monitor and display current peak levels only when a recording device
	//is opened. It is easy to see loopback device levels (if any music playing),
	//but must explicitly initialize all microphones, etc.
	BOOL InitRecordingDevices(const DevicesArray& devices);
	BOOL FreeRecordingDevices(const DevicesArray& devices);


	class CurrentThreadDevice
	{
	public:
		CurrentThreadDevice(int device);
		//{
		//	m_oldDevice = BASS_WASAPI_GetDevice();
		//	BASS_WASAPI_SetDevice(device);
		//}
		~CurrentThreadDevice();
		//{
		//	BASS_WASAPI_SetDevice(m_oldDevice);
		//}
	private:
		int m_oldDevice;
	};

} // namespace Bass

/////////////////////////////////////////////////////////////////////////////

#endif // _WASAPI_HELPERS_H
