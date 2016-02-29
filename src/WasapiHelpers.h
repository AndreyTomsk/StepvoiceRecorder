#ifndef _WASAPI_HELPERS_H
#define _WASAPI_HELPERS_H
#pragma once

#include <vector>
#include <MMDeviceApi.h> //for EDataFlow, etc.
#include <Audioclient.h> //for IAudioClient, IAudioCaptureClient

/////////////////////////////////////////////////////////////////////////////

typedef void (*OnVolumeChanged)(float curVolume, void* user);
typedef std::pair<OnVolumeChanged, void*> PairVolumeChanged;

class IWasapiRecorder
{
public:
	virtual DWORD GetActualFrequency() const = 0;
	virtual DWORD GetActualChannelCount() const = 0;

	virtual BOOL Start() = 0;
	virtual BOOL Pause() = 0;
	virtual BOOL Stop() = 0;
	virtual BOOL IsStarted() const = 0;
	virtual BOOL IsPaused() const = 0;
	virtual BOOL IsStopped() const = 0;

	virtual BOOL  VolumeControlAvailable() const = 0;
	virtual float GetVolume() const = 0;
	virtual BOOL  SetVolume(float volume) = 0; //0..1
	void AddEvent(OnVolumeChanged ove, void* userData);

	virtual float GetPeakLevel(int channel) const = 0; //0 = first channel, -1 = all channels
	virtual DWORD GetData(void* buffer, DWORD lengthBytes) const = 0; //returns -1 if error

	//For compatibility with GraphWnd callback
	virtual DWORD GetChannelData(int channel, float* buffer, int bufferSize) = 0;

protected:
	void CallVolumeChangedEvents(float curVolume) const;
private:
	std::vector<PairVolumeChanged> m_volumeEvents;
};

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
	//BOOL InitRecordingDevices(const DevicesArray& devices);
	//BOOL FreeRecordingDevices(const DevicesArray& devices);

	HRESULT ActivateDevice(const CString& strDeviceID, IAudioClient **audioClient);

	//Saving device name with ID ("<id>|<name>\n<id>|<name>\n...") to fix
	//several different devices with same name problem.
	CString DevicesToString(WasapiHelpers::DevicesArray arr);
	DevicesArray DevicesFromString(CString strNames, const DevicesArray& allValidDevices);

/////////////////////////////////////////////////////////////////////////////
} // namespace WasapiHelpers

#endif // _WASAPI_HELPERS_H
