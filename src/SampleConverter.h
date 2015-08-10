#ifndef _SAMPLE_CONVERTER_H
#define _SAMPLE_CONVERTER_H
#pragma once

namespace SampleConverter
{
//For a resampling srcFreq-->dstFreq, this function calculates a source
//sample count, based on a required sample count in destination. Currently
//implemented for cases when srcFreq > dstFreq. Example:
//  1118 samples at 48kHz equals to 1024 at 44.1. (so function returns 1118).
int GetSrcSampleCount(int srcFreq, int dstFreq, int dstSampleCount);

//Just verifying that resampling will produce exacly testDstSampleCount
//samples. Using the same countin algorithm as in the ConvertSamples func.
bool VerifyDstSampleCount(int srcFreq, int srcSampleCount, int dstFreq,
	int testDstSampleCount);

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
} //namespace SampleConverter

#endif // _SAMPLE_CONVERTER_H
