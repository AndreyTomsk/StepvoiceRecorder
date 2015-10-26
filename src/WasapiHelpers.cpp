#include "stdafx.h"
#include <basswasapi.h>
#include <algorithm> //for std::find
#include "WasapiHelpers.h"
#include "StrUtils.h"
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

		//TODO: can be lengthy operation (HDMI devices), refactor/move to thread.
		//WriteDbg() << __FUNCTION__ << " ::2.1, device name=" << p.second;
		result = BASS_WASAPI_Init(p.first, 44100, 2, BASS_WASAPI_AUTOFORMAT, 0.5, 0, EmptyProc, NULL);
		result = BASS_WASAPI_Start();
		//WriteDbg() << __FUNCTION__ << " ::2.2";

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

const TCHAR DEVICES_DELIMETER = _T('\n');
const TCHAR ID_DELIMETER = _T('|');

struct MatchByIDName
{
	MatchByIDName(const CString& id, const CString& name)
		:m_requiredID(StrUtils::FromString(id))
		,m_requiredName(name)
	{
	}
	bool operator()(DeviceIdNamePair p) const
	{
		return p.first == m_requiredID && p.second == m_requiredName;
	}

private:
	CString m_requiredName;
	DWORD m_requiredID;
};
//---------------------------------------------------------------------------

CString DevicesToString(WasapiHelpers::DevicesArray arr)
{
	std::vector<CString> deviceNames;
	for (size_t i = 0; i < arr.size(); i++)
	{
		const CString id = StrUtils::ToString(arr[i].first);
		const CString name = arr[i].second;
		deviceNames.push_back(id + ID_DELIMETER + name);
	}

	return StrUtils::Join(deviceNames, DEVICES_DELIMETER);
}
//---------------------------------------------------------------------------

DevicesArray DevicesFromString(CString strNames, const DevicesArray& validDevices)
{
	DevicesArray result;

	std::vector<CString> devices = StrUtils::Split(strNames, DEVICES_DELIMETER);
	for (size_t i = 0; i < devices.size(); i++)
	{
		//Extracting device id and name from full string.
		std::vector<CString> idName = StrUtils::Split(devices[i], ID_DELIMETER);
		if (idName.size() != 2)
			continue;

		MatchByIDName match(idName[0], idName[1]);
		const DevicesArray::const_iterator it =
			std::find_if(validDevices.begin(), validDevices.end(), match);

		if (it != validDevices.end())
			result.push_back(*it);
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////////

} // namespace WasapiHelpers
