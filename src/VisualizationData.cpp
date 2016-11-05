/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "VisualizationData.h"

////////////////////////////////////////////////////////////////////////////////

VisualizationData::VisualizationData(int a_sound_sample_freq, int a_sound_channels)
	:m_sample_buffer(NULL)
	,m_current_buffer_size(0)
	,SAMPLE_BUFFER_SIZE(4096)
	,SAMPLE_FREQ(a_sound_sample_freq)
	,CHANNEL_COUNT(a_sound_channels)
	,m_cur_level_l(0.0)
	,m_cur_level_r(0.0)
{
	m_sample_buffer = new float[SAMPLE_BUFFER_SIZE];
	ASSERT(m_sample_buffer);
}
//-----------------------------------------------------------------------------

VisualizationData::~VisualizationData()
{
	delete [] m_sample_buffer;
}
//-----------------------------------------------------------------------------

void VisualizationData::SetSourceBuffer(float* a_buffer, int a_length_bytes)
{
	CMyLock lock(m_sync_object);

	int l_bytes_2copy = min(a_length_bytes, int(SAMPLE_BUFFER_SIZE * sizeof(float)));
	memcpy(m_sample_buffer, a_buffer, l_bytes_2copy);
	m_current_buffer_size = l_bytes_2copy / sizeof(float);

	m_cur_level_l = m_cur_level_r = 0.0;
	for (int i = 0; i+1 < m_current_buffer_size; i += 8) // 4096 / 8 = 512 steps
	{
		m_cur_level_l = max(m_cur_level_l, m_sample_buffer[i]);
		m_cur_level_r = max(m_cur_level_r, m_sample_buffer[i + 1]);
	}
}
//-----------------------------------------------------------------------------

float VisualizationData::GetPeaksLevel(int a_channel)
{
	return (a_channel == 0) ? m_cur_level_l : m_cur_level_r;
}
//-----------------------------------------------------------------------------

int VisualizationData::GetLinesLevel(int a_channel, float* a_buffer, int a_size)
{
	CMyLock lock(m_sync_object);

	// Sometimes source buffer contains only a few new samples. In this case
	// returning whole buffer (new data at beginning, old - at end) for wave display.
	//int l_size_4copy = min(m_current_buffer_size, a_size);

	int l_size_4copy = min(max(m_current_buffer_size, SAMPLE_BUFFER_SIZE), a_size);

	int l_dst_count = 0;
	for (int i = a_channel; i < l_size_4copy; i += CHANNEL_COUNT)
		a_buffer[l_dst_count++] = m_sample_buffer[i];

	return l_dst_count;
}
//-----------------------------------------------------------------------------
