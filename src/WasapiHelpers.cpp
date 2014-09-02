#include "stdafx.h"
#include <basswasapi.h>
#include "WasapiHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

namespace WasapiHelpers
{
//---------------------------------------------------------------------------

DevicesArray GetWasapiDevicesList()
{
	DevicesArray result;

	BASS_WASAPI_DEVICEINFO info;
	for (int id = 0; BASS_WASAPI_GetDeviceInfo(id, &info); id++)
	{
		const BOOL isEnabled = info.flags & BASS_DEVICE_ENABLED;
		const BOOL isInputDevice = info.flags & BASS_DEVICE_INPUT;
		const BOOL isLoopback = info.flags & BASS_DEVICE_LOOPBACK;

		if (isEnabled && (isInputDevice || isLoopback))
			result.push_back(DeviceIdNamePair(id, info.name));
	}
	return result;
}
//---------------------------------------------------------------------------

DeviceIdNamePair GetDefaultRecordingDevice()
{
	BASS_WASAPI_DEVICEINFO info;
	for (int id = 0; BASS_WASAPI_GetDeviceInfo(id, &info); id++)
	{
		const BOOL isEnabled = info.flags & BASS_DEVICE_ENABLED;
		const BOOL isDefault = info.flags & BASS_DEVICE_DEFAULT;
		const BOOL isInputDevice = info.flags & BASS_DEVICE_INPUT;

		if (isEnabled && isInputDevice && isDefault)
			return DeviceIdNamePair(id, info.name);
	}

	return DeviceIdNamePair(0xFFFF, _T(""));
}
//---------------------------------------------------------------------------

static DWORD CALLBACK EmptyProc(void* , DWORD , void* ) { return 1; }

BOOL GetDeviceActualData(int device, DWORD freq, DWORD chans,
	DWORD& actualFreq, DWORD& actualChans)
{
	BASS_WASAPI_Init(device, freq, chans, BASS_WASAPI_AUTOFORMAT, 0.5, 0, EmptyProc, NULL);

	BASS_WASAPI_INFO info;
	BASS_WASAPI_GetInfo(&info);
	actualFreq = info.freq;
	actualChans = info.chans;

	return BASS_WASAPI_Free();
}

/////////////////////////////////////////////////////////////////////////////

} // namespace WasapiHelpers
