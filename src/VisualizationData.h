/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#ifndef _VISUALIZATION_DATA_H
#define _VISUALIZATION_DATA_H

#include "SyncObjects.h"

/// Counting visualization parameters from an input buffer

////////////////////////////////////////////////////////////////////////////////
class VisualizationData
{
public:
	VisualizationData(int a_sound_sample_freq, int a_sound_channels);
	~VisualizationData();

	void  SetSourceBuffer(float* a_buffer, int a_length_bytes);
	float GetPeaksLevel(int a_channel);
	int   GetLinesLevel(int a_channel, float* a_buffer, int a_size); // returns copied size

private:
	const int SAMPLE_BUFFER_SIZE;
	const int SAMPLE_FREQ;
	const int CHANNEL_COUNT;

	float* m_sample_buffer;
	int m_current_buffer_size;

	float m_cur_level_l;
	float m_cur_level_r;

	CMyCriticalSection m_sync_object;
};

#endif //_VISUALIZATION_DATA_H
