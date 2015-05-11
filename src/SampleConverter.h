#ifndef _SAMPLE_CONVERTER_H
#define _SAMPLE_CONVERTER_H
#pragma once

/////////////////////////////////////////////////////////////////////////////

struct SampleBuffer
{
	SampleBuffer(int freq, int chans, float* buffer);

	int frequency;
	int channels;
	float* data;
	unsigned curSamples;
	unsigned maxSamples;
};

/////////////////////////////////////////////////////////////////////////////

//Downsampling function (freqIn > freqOut).
void ConvertSamples(const SampleBuffer& in, SampleBuffer& out);

//Downsampling function (freqIn > freqOut).
//Returns actual buffer size.
//int ConvertSamples(float* buffer, int size, int freqIn, int freqOut);
/*
class CSampleConvertor
{
public:
	CSampleConvertor(const SampleBuffer& sb);
	~CSampleConvertor();

	int Convert(char* dstBuffer, int dstBytes, int dstFreq, int dstChannels) const;

private:
	SampleBuffer m_sb;
};
*/

/////////////////////////////////////////////////////////////////////////////

#endif // _SAMPLE_CONVERTER_H
