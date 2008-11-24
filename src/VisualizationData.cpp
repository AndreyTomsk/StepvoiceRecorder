
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

	short l_level_l = 0;
	l_level_r = 0;

	short* l_buffer_ptr = (short*)m_rec_buffer;
	int    l_buffer_size = m_rec_length / sizeof(short);

	// Making simplified count because we know that buffer is changed fast enough

	for (int i = 0; i < l_buffer_size; i += 8) // +8 instead of +2 is for speed
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

	float l_proportion = (float)l_buffer_size / a_size;

	int l_dst_index = 0;
	for (; l_dst_index < a_size; l_dst_index++)
	{
		int l_src_index = int(l_proportion * l_dst_index);
		l_src_index = (l_src_index / CHANNEL_COUNT) * CHANNEL_COUNT + a_channel;

		// Making check for a correct index (useful when l_proportion is < 1)
		if (l_src_index >= l_buffer_size)
			continue;

		a_buffer[l_dst_index] = (float)l_buffer_ptr[l_src_index] / MAX_SHORT;
	}
	return l_dst_index;
}
