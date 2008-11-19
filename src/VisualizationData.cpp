
#include "stdafx.h"
#include "VisualizationData.h"

const int MAX_SHORT = 32768;

VisualizationData::VisualizationData(int a_sound_sample_freq,
											 int a_sound_channels)
	:BUFFER_LENGTH(102400)
	,SAMPLE_FREQ(a_sound_sample_freq)
	,CHANNEL_COUNT(a_sound_channels)
	,m_rec_buffer(new char[BUFFER_LENGTH])
	,m_rec_length(0)
	,m_rec_msec_begin(0)
{
	ASSERT(m_rec_buffer);
}

VisualizationData::~VisualizationData()
{
	delete [] m_rec_buffer;
	m_rec_buffer = NULL;
}

void VisualizationData::SetSourceBuffer(void* a_buffer, int a_length)
{
	CMyLock lock(m_sync_object);

	///NOTE: Assuming what we have a 16-bit sound buffer.
	ASSERT(a_length <= BUFFER_LENGTH);
	memcpy(m_rec_buffer, a_buffer, a_length);

	m_rec_length = a_length;
	m_rec_msec_begin = ::GetTickCount();
}

float VisualizationData::GetPeaksLevel(int a_channel)
{
	CMyLock lock(m_sync_object);
	if (m_rec_msec_begin == 0)
		return 0;

	ASSERT(a_channel < CHANNEL_COUNT);
	static short l_level_r = 0;
	if (a_channel == 1)
		return float(abs(l_level_r)) / MAX_SHORT;

	int l_check_length = min(1764, m_rec_length / sizeof(short)); // 20ms for 44,1kHz
	l_check_length = (l_check_length / 2) * 2;

	short l_level_l = 0;
	l_level_r = 0;
	short* l_buffer_ptr = (short*)m_rec_buffer;

	int l_request_msec = ::GetTickCount();
	int l_rec_offset = (l_request_msec - m_rec_msec_begin) * 88; // precalculated bytes/msec rate
	if (l_rec_offset + l_check_length >= m_rec_length)
		return 0;

	for (int i = l_rec_offset; i < l_rec_offset + l_check_length; i += 2)
	{
		l_level_l = max(l_level_l, l_buffer_ptr[i]);
		l_level_r = max(l_level_r, l_buffer_ptr[i + 1]);
	}
	return float(abs(l_level_l)) / MAX_SHORT;
}

int VisualizationData::GetLinesLevel(int a_channel, float* a_buffer, int a_size)
{
	CMyLock lock(m_sync_object);

	short* l_buffer_ptr = (short*)m_rec_buffer;
	int    l_buffer_size = m_rec_length / sizeof(short);

	///BUG: this is no good (where -1024)!
	int l_numbers_2copy = min(l_buffer_size - a_channel, a_size - 1024);
	int l_src_offset = a_channel;
	int l_dst_offset = 0;

	while (l_dst_offset < l_numbers_2copy)
	{
		a_buffer[l_dst_offset] = (float)l_buffer_ptr[l_src_offset] / MAX_SHORT;
		l_src_offset += CHANNEL_COUNT;
		l_dst_offset += 1;
	}
	return l_numbers_2copy;
}
