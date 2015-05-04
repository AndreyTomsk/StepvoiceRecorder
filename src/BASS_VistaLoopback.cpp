// BASS stream for loopback recording in Windows Vista.
// Copyright (c) 2008 Andrew Firsov.

#include "stdafx.h"
#include <basswasapi.h>
#include <functiondiscoverykeys.h>
#include "BASS_VistaLoopback.h"

#define EIF(x) if (FAILED(hr=(x))) { goto Exit; }	// Exit If Failed.
#define SAFE_RELEASE(x) { if (x != NULL) { x.Release(); x = NULL; } }

#define MFTIMES_PER_MILLISEC  10000

////////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK BassVistaLoopback::LoopbackStreamProc(HSTREAM /*a_handle*/,
	void* a_buffer, DWORD a_length, void* a_user)
{
	BassVistaLoopback* l_this = (BassVistaLoopback *)a_user;
	ASSERT(l_this);

	HRESULT hr;

	// Applying sound delay for 0.1 sec (currently).
	{
		UINT32 paddingFrameCount;
		hr = l_this->m_audio_client->GetCurrentPadding(&paddingFrameCount);
		if (!paddingFrameCount || (paddingFrameCount < l_this->m_buffer_size/4 && !l_this->m_buffer_delay))
		{
			//if (paddingFrameCount)
			//	::OutputDebugString(__FUNCTION__ " ::Delaying (wait 1/4 buffer)");
			//else
			//	::OutputDebugString(__FUNCTION__ " ::Delaying (no frames in buffer");

			l_this->m_buffer_delay = false;
			ZeroMemory(a_buffer, a_length);
			return a_length;
		}
		else
			l_this->m_buffer_delay = true;
	}

	BYTE*  l_dst_buffer = (BYTE*)a_buffer;
	UINT32 l_dst_length = a_length;

	// NOTE: we fill the required buffer completely
	while (l_dst_length)
	{
		// 1. Getting new chunk
		// 2. If we don't have any sounds, then just making the remaining part
		//      of the buffer silent and exit
		// 3. Counting data length for copying
		// 4. Filling destination buffer
		// 5. Updating variables
		// 6. If l_dst_length, then repeat

		BYTE*   l_src_buffer = NULL;
		DWORD   l_src_flags = 0;
		UINT32  l_src_frames_available = 0;
		UINT32& l_src_offset = l_this->m_src_offset;

		hr = l_this->m_capture_client->GetBuffer(&l_src_buffer,
			&l_src_frames_available, &l_src_flags, NULL, NULL);

		if (!l_src_frames_available)
		{
			if (l_dst_length == a_length)
				::OutputDebugString(__FUNCTION__ " ::Empty buffer");
			else
				::OutputDebugString(__FUNCTION__ " ::Empty buffer (partially filled)");

			ZeroMemory(l_dst_buffer, l_dst_length);
			hr = l_this->m_capture_client->ReleaseBuffer(l_src_frames_available);
			// Return filled length if no more frames.
			return (l_dst_length == a_length) ? a_length : a_length - l_dst_length;
		}

		UINT32 l_src_bytes_available = l_src_frames_available * l_this->m_wfx->nBlockAlign;

		if (l_src_bytes_available > a_length)
			::OutputDebugString(__FUNCTION__ " :: TOO MANY FRAMES!");

		UINT32 l_bytes_4copy = min(l_dst_length, l_src_bytes_available - l_src_offset);

		memcpy(l_dst_buffer, l_src_buffer + l_src_offset, l_bytes_4copy);

		l_dst_length -= l_bytes_4copy;
		l_dst_buffer += l_bytes_4copy;
		l_src_offset += l_bytes_4copy;

		ASSERT(l_src_offset <= l_src_bytes_available);
		if (l_src_offset == l_src_bytes_available)
			l_src_offset = 0;	// clearing offset if all copied

		// Asking for same chunk (0) if it wasn't fully copied
		hr = l_this->m_capture_client->ReleaseBuffer(
			(l_src_offset) ? 0 : l_src_frames_available);
	}
	return a_length;
}
//---------------------------------------------------------------------------

BassVistaLoopback::BassVistaLoopback(int a_device)
	:m_wfx(NULL)
	,m_loopback_stream(0)
	,m_src_offset(0)
	,m_buffer_delay(false)
	,m_buffer_size(0)
{
	USES_CONVERSION;
	HRESULT hr;

	/*
	if (a_device < 0)
	{
		EIF(GetDefaultDevice(eRender, &m_audio_client));
	}
	else
	{
		// Taking device, based on driver name, not ID (BASS enumeration doesn't
		// equal with WASAPI's). Again, real device IDs starts from 1 in BASS.
		BASS_DEVICEINFO info;
		BOOL result = BASS_GetDeviceInfo(a_device + 1, &info);
		ASSERT(result);

		EIF(GetDevice(A2W(info.driver), eRender, &m_audio_client));
	}
	*/

	// Taking device, based on driver name, not ID (BASS enumeration doesn't
	// equal with WASAPI's). Again, real device IDs starts from 1 in BASS.
	BASS_WASAPI_DEVICEINFO info;
	BOOL result = BASS_WASAPI_GetDeviceInfo(a_device, &info);
	ASSERT(result);
	EIF(GetDevice(A2W(info.id), eAll, &m_audio_client));

	const DWORD streamFlags = (info.flags & BASS_DEVICE_LOOPBACK) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0;
	const REFERENCE_TIME bufferDuration = 500 * MFTIMES_PER_MILLISEC;

	EIF(m_audio_client->GetMixFormat(&m_wfx));
	EIF(m_audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, bufferDuration, 0, m_wfx, NULL));
	EIF(m_audio_client->GetBufferSize(&m_buffer_size)); //in frames

	EIF(m_audio_client->GetService(__uuidof(IAudioCaptureClient),
		(void**)&m_capture_client));
	EIF(m_audio_client->Start());

	m_loopback_stream = BASS_StreamCreate(
		GetActualFrequency(),
		GetActualChannelCount(),
		BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE,
		LoopbackStreamProc,
		this);
	
Exit:
	return;
}
//---------------------------------------------------------------------------

BassVistaLoopback::~BassVistaLoopback()
{
	if (m_audio_client)
	{
		HRESULT hr = m_audio_client->Stop();
		ASSERT(!FAILED(hr));

		BASS_StreamFree(m_loopback_stream);
		CoTaskMemFree(m_wfx);
	}
}
//---------------------------------------------------------------------------

DWORD BassVistaLoopback::GetActualFrequency() const
{
	return (m_wfx != NULL) ? m_wfx->nSamplesPerSec : 0;
}
//---------------------------------------------------------------------------

DWORD BassVistaLoopback::GetActualChannelCount() const
{
	return (m_wfx != NULL) ? m_wfx->nChannels : 0;
}
//---------------------------------------------------------------------------

HSTREAM BassVistaLoopback::GetLoopbackStream() const
{
	return m_loopback_stream;
}
//---------------------------------------------------------------------------

HRESULT BassVistaLoopback::GetDefaultDevice(EDataFlow a_flow, IAudioClient **a_client)
{
	CComPtr<IMMDeviceEnumerator> l_enumerator;
	CComPtr<IMMDevice> l_device;

	HRESULT hr;
	EIF(l_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
	EIF(l_enumerator->GetDefaultAudioEndpoint(a_flow, eConsole, &l_device));
	EIF(l_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
		NULL, reinterpret_cast<void**>(a_client)));

	/*
	// Test code for getting device parameters 
	CComPtr<IPropertyStore> l_properties;
	l_device->OpenPropertyStore(STGM_READ, &l_properties);

	PROPVARIANT varName;
	PropVariantInit(&varName);
	EIF(l_properties->GetValue(PKEY_DeviceInterface_FriendlyName, &varName));
	EIF(l_properties->GetValue(PKEY_Device_DeviceDesc, &varName));
	EIF(l_properties->GetValue(PKEY_Device_FriendlyName, &varName));
	PropVariantClear(&varName);
	*/
Exit:
	return hr;
}
//---------------------------------------------------------------------------

HRESULT BassVistaLoopback::GetDevice(LPCWSTR a_device_driver, EDataFlow a_flow,
									 IAudioClient **a_client)
{
	CComPtr<IMMDeviceEnumerator> l_enumerator;
	CComPtr<IMMDeviceCollection> l_device_collection;
	CComPtr<IMMDevice> l_device;

	LPWSTR l_device_driver = NULL;
	UINT l_device_count = 0;
	UINT l_device_id = -1;

	HRESULT hr;
	EIF(l_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
	EIF(l_enumerator->EnumAudioEndpoints(a_flow, DEVICE_STATE_ACTIVE, &l_device_collection));

	EIF(l_device_collection->GetCount(&l_device_count));
	for (UINT i = 0; i < l_device_count; i++)
	{
		EIF(l_device_collection->Item(i, &l_device));
		EIF(l_device->GetId(&l_device_driver));
		SAFE_RELEASE(l_device);

		if (CString(a_device_driver) == CString(l_device_driver))
			l_device_id = i;
		CoTaskMemFree(l_device_driver);
		if (l_device_id != -1)
			break;
	}

	EIF(l_device_collection->Item(l_device_id, &l_device));
	EIF(l_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
		NULL, reinterpret_cast<void**>(a_client)));
Exit:
	return hr;
}
//---------------------------------------------------------------------------

HRESULT BassVistaLoopback::GetDevice(int a_device_id, EDataFlow a_flow,
									 IAudioClient **a_client)
{
	CComPtr<IMMDeviceEnumerator> l_enumerator;
	CComPtr<IMMDeviceCollection> l_device_collection;
	CComPtr<IMMDevice> l_device;

	HRESULT hr;
	EIF(l_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
	EIF(l_enumerator->EnumAudioEndpoints(a_flow, DEVICE_STATE_ACTIVE, &l_device_collection));

	EIF(l_device_collection->Item(a_device_id, &l_device));
	EIF(l_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
		NULL, reinterpret_cast<void**>(a_client)));

Exit:
	return hr;
}
//---------------------------------------------------------------------------

HRESULT BassVistaLoopback::GetPlaybackDevicesNames(CStringArray& arr)
{
	arr.RemoveAll();

	CComPtr<IMMDeviceEnumerator> l_enumerator;
	CComPtr<IMMDeviceCollection> l_device_collection;

	HRESULT hr;
	EIF(l_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
	EIF(l_enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &l_device_collection));

	UINT l_device_count;
	EIF(l_device_collection->GetCount(&l_device_count));
	for (int i = 0; i < l_device_count; i++)
	{
		CComPtr<IMMDevice> l_device;
		EIF(l_device_collection->Item(i, &l_device));

		CComPtr<IPropertyStore> l_properties;
		l_device->OpenPropertyStore(STGM_READ, &l_properties);

		PROPVARIANT varName;
		PropVariantInit(&varName);
		EIF(l_properties->GetValue(PKEY_Device_FriendlyName, &varName));
		arr.Add(varName.pwszVal);
		PropVariantClear(&varName);
	}

Exit:
	return hr;
}
//---------------------------------------------------------------------------
