#include "stdafx.h"
#include <basswasapi.h>
#include "WasapiHelpers.h"
#include "common.h" //for EIF,SAFE_RELEASE macro

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

namespace WasapiHelpers
{
//---------------------------------------------------------------------------

const DeviceIdNamePair EMPTY_DEVICE = DeviceIdNamePair(0xFFFF, _T(""));

//---------------------------------------------------------------------------

DevicesArray GetRecordingDevicesList()
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
	//NOTE: seems that loopback devices cannot be system default.
	DeviceIdNamePair defaultRecordingDevice = EMPTY_DEVICE;
	DeviceIdNamePair defaultLoopbackDevice = EMPTY_DEVICE;

	BASS_WASAPI_DEVICEINFO info;
	for (int id = 0; BASS_WASAPI_GetDeviceInfo(id, &info); id++)
	{
		const BOOL isEnabled = info.flags & BASS_DEVICE_ENABLED;
		const BOOL isDefault = info.flags & BASS_DEVICE_DEFAULT;
		const BOOL isInputDevice = info.flags & BASS_DEVICE_INPUT;
		const BOOL isLoopback = info.flags & BASS_DEVICE_LOOPBACK;

		if (isEnabled && isLoopback)
			defaultLoopbackDevice = DeviceIdNamePair(id, info.name);

		//Input device has most priority. Break if found.
		if (isEnabled && isInputDevice && isDefault)
		{
			defaultRecordingDevice = DeviceIdNamePair(id, info.name);
			break;
		}
	}

	return (defaultRecordingDevice != EMPTY_DEVICE) ?
		defaultRecordingDevice : defaultLoopbackDevice;
}
//---------------------------------------------------------------------------

static DWORD CALLBACK EmptyProc(void* , DWORD , void* ) { return 1; } //0 stops recording
//---------------------------------------------------------------------------

BOOL GetDeviceActualData(int device, DWORD freq, DWORD chans,
	DWORD& actualFreq, DWORD& actualChans)
{
	BOOL result = BASS_WASAPI_Init(device, freq, chans, BASS_WASAPI_AUTOFORMAT, 0.5, 0, EmptyProc, NULL);
	ASSERT(result);

	BASS_WASAPI_INFO info;
	BASS_WASAPI_GetInfo(&info);
	actualFreq = info.freq;
	actualChans = info.chans;

	return BASS_WASAPI_Free();
}
//---------------------------------------------------------------------------

BOOL InitRecordingDevices(const DevicesArray& devices)
{
	BOOL result = FALSE;

	DevicesArray::const_iterator it = devices.begin();
	while (it != devices.end())
	{
		DeviceIdNamePair p = *it++;
		result = BASS_WASAPI_Init(p.first, 44100, 2, BASS_WASAPI_AUTOFORMAT, 0.5, 0, EmptyProc, NULL);
		result = BASS_WASAPI_Start();

		//BASS_WASAPI_AUTOFORMAT helps - my webcam's microphone (Logitech C270)
		//initializes recording with 48000Hz, Mono. See GetDeviceActualData above.
	}
	return result;
}
//---------------------------------------------------------------------------

BOOL FreeRecordingDevices(const DevicesArray& devices)
{
	BOOL result = FALSE;

	DevicesArray::const_iterator it = devices.begin();
	while (it != devices.end())
	{
		DeviceIdNamePair p = *it++;
		result = BASS_WASAPI_SetDevice(p.first);
		result = BASS_WASAPI_Free(); //frees device, currently set for this thread
	}
	return result;
}
//---------------------------------------------------------------------------

HRESULT GetActiveDevice(const CString& strDeviceID, IAudioClient **audioClient)
{
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	CComPtr<IMMDeviceCollection> deviceCollection;
	CComPtr<IMMDevice> device;

	LPWSTR strDeviceIDfromWasapi = NULL;
	UINT allDeviceCount = 0;

	HRESULT hr = S_OK;
	EIF(deviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
	EIF(deviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollection));
	EIF(deviceCollection->GetCount(&allDeviceCount));

	for (UINT i = 0; i < allDeviceCount; i++)
	{
		SAFE_RELEASE(device);
		EIF(deviceCollection->Item(i, &device));
		EIF(device->GetId(&strDeviceIDfromWasapi));

		const bool deviceFound = CString(strDeviceIDfromWasapi) == strDeviceID;
		CoTaskMemFree(strDeviceIDfromWasapi);
		if (deviceFound)
		{
			EIF(device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
				NULL, reinterpret_cast<void**>(audioClient)));
			break;
		}
	}
	hr = (*audioClient != NULL);
Exit:
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

} // namespace WasapiHelpers
