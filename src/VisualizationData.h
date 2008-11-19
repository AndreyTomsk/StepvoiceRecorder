
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

	void  SetSourceBuffer(void* a_buffer, int a_length);
	float GetPeaksLevel(int a_channel);
	int   GetLinesLevel(int a_channel, float* a_buffer, int a_size);

private:
	const int BUFFER_LENGTH;
	const int SAMPLE_FREQ;
	const int CHANNEL_COUNT;

	char* m_rec_buffer;
	int  m_rec_length;
	int  m_rec_msec_begin;
	CMyCriticalSection m_sync_object;
};

#endif //_VISUALIZATION_DATA_H

