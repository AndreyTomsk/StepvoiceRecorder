#include "stdafx.h"
#include <bass.h>
#include <basswasapi.h>
#include <math.h>
#include "Bass_Functions.h"

/////////////////////////////////////////////////////////////////////////////

namespace Bass
{
//---------------------------------------------------------------------------

float GetMaxPeakDB(DWORD a_handle)
{
	DWORD l_ch_level = BASS_ChannelGetLevel(a_handle); // 0..32768
	ASSERT(l_ch_level != -1);

	DWORD l_level = max(LOWORD(l_ch_level), HIWORD(l_ch_level));
	return 20 * log10(float(l_level) / 32768);
}
//---------------------------------------------------------------------------

void CALLBACK LoopbackStreamDSP(DWORD /*a_dsp_handle*/, DWORD /*a_channel*/,
	void *a_buffer, DWORD a_length_bytes, void *a_user)
{
	// Loopback is a decoding stream and have same parameters (freq, channels)
	// as a main recording stream, for direct copying.
	ASSERT(BASS_GetConfig(BASS_CONFIG_FLOATDSP) != -1); // float samples check
	ASSERT (a_user != NULL);	

	const DWORD TEMP_BUFFER_SIZE = 4096;
	static float l_tmp_buffer[TEMP_BUFFER_SIZE] = {0.0};
	HSTREAM l_src_stream = *((HSTREAM*)a_user);
	DWORD l_copy_offset = 0;

	while (a_length_bytes > 0)
	{
		DWORD l_copy_count = min(TEMP_BUFFER_SIZE * sizeof(float), a_length_bytes);
		DWORD l_bytes_copied = BASS_ChannelGetData(l_src_stream, l_tmp_buffer,
			l_copy_count | BASS_DATA_FLOAT);
		
		if (l_bytes_copied == -1)
		{
			//int l_debug = BASS_ErrorGetCode();
			ASSERT(false);
			return;
		}

		float* l_dst_buffer = static_cast<float*>(a_buffer) + l_copy_offset/sizeof(float);
		for (int i = 0, count = l_bytes_copied / sizeof(float); i < count; i++)
			l_dst_buffer[i] = max(-1.0, min((l_dst_buffer[i] + l_tmp_buffer[i]), 1.0));

		//memcpy((BYTE*)a_buffer + l_copy_offset, l_tmp_buffer, l_bytes_copied);
		a_length_bytes -= l_bytes_copied;
		l_copy_offset += l_bytes_copied;
	}

/*
	//1. Fill buffer with required length from Loopback stream (a_user)
	const int BUFFER_LENGTH = 128 * 1024;
	ASSERT(a_length <= BUFFER_LENGTH);
	ASSERT(a_user);

	static char l_src_buffer[BUFFER_LENGTH] = {0}; //128k buffer
	HSTREAM l_src_stream = *((HSTREAM*)a_user);

	DWORD l_length = BASS_ChannelGetData(l_src_stream, l_src_buffer, a_length);
	if (l_length == -1)
	{
		int l_debug = BASS_ErrorGetCode();
		l_debug = 0;
		return;
	}
	ASSERT(l_length == a_length);

	//2. Replace data in the recording buffer (assuming we have float samples).
	char* l_dst_buffer = (char*)a_buffer;
	for (DWORD i = 0; i < a_length; i++)
	l_dst_buffer[i] = l_src_buffer[i];
*/
	/*
	//2. Mix data with destination buffer (assuming we have float samples).
	float* l_dst_buffer = (float*)a_buffer;
	float* l_src_buffer_float = (float*)l_src_buffer;

	for (int i = 0, count = a_length / sizeof(float); i < count; i++)
	l_dst_buffer[i] = max(-1.0, min((l_dst_buffer[i] + l_src_buffer_float[i]), 1.0));
	*/
}
//---------------------------------------------------------------------------

void CALLBACK StreamMuteDSP(DWORD /*handle*/, DWORD /*channel*/, void *buffer,
							DWORD length, void* /*user*/)
{
	memset(buffer, 0, length);
}
//---------------------------------------------------------------------------

bool IsPlaybackDeviceValid(int a_device)
{
	BASS_DEVICEINFO info;
	BOOL result = BASS_GetDeviceInfo(a_device + 1, &info);
	return result && (info.flags&BASS_DEVICE_ENABLED);
}
//---------------------------------------------------------------------------

bool IsRecordingDeviceValid(int a_device)
{
	BASS_DEVICEINFO info;
	BOOL result = BASS_RecordGetDeviceInfo(a_device + 1, &info);
	return result && (info.flags&BASS_DEVICE_ENABLED);
}

/////////////////////////////////////////////////////////////////////////////

} // namespace Bass
