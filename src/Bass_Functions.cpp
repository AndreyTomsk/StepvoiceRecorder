
#include "stdafx.h"
#include "Bass_Functions.h"
#include <math.h>

namespace Bass
{
//------------------------------------------------------------------------------
float GetMaxPeakDB(HRECORD a_handle)
{
	//BASS_CHANNELINFO l_ci;
	//BOOL l_result = BASS_ChannelGetInfo(a_handle, &l_ci);
	//ASSERT(l_result);

	//int l_max_possible = 0x7FFF; // 16-bit as default, 0xFFFF/2
	//if (l_ci.flags & BASS_SAMPLE_8BITS)
	//	l_max_possible = 0x7F; //0xFF/2
	//else if (l_ci.flags & BASS_SAMPLE_FLOAT)
	//	l_max_possible = 1;

	DWORD l_ch_level = BASS_ChannelGetLevel(a_handle);
	ASSERT(l_ch_level != -1);

	DWORD l_level = max(LOWORD(l_ch_level), HIWORD(l_ch_level)); // 0-32768=0x8000.
	const DWORD l_max_possible = 0x8000;

	return 20 * log10(float(l_level) / l_max_possible);
}

//------------------------------------------------------------------------------
void CALLBACK LoopbackStreamDSP(HDSP /*a_dsp_handle*/, DWORD /*a_channel*/,
								void *a_buffer, DWORD a_length, void *a_user)
{
	// Loopback is a decoding stream and have same parameters (freq, channels)
	// as a main recording stream, for direct copying.

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

	/*
	//2. Mix data with destination buffer (assuming we have float samples).
	float* l_dst_buffer = (float*)a_buffer;
	float* l_src_buffer_float = (float*)l_src_buffer;

	for (int i = 0, count = a_length / sizeof(float); i < count; i++)
		l_dst_buffer[i] = max(-1.0, min((l_dst_buffer[i] + l_src_buffer_float[i]), 1.0));
	*/

	//2. Replace data in the recording buffer (assuming we have float samples).
	char* l_dst_buffer = (char*)a_buffer;
	for (DWORD i = 0; i < a_length; i++)
	l_dst_buffer[i] = l_src_buffer[i];
}

//------------------------------------------------------------------------------
} // namespace Bass
