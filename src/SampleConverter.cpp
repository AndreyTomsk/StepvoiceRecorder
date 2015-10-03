#include "stdafx.h"
#include "SampleConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SampleConverter
{
//---------------------------------------------------------------------------

int GetSrcSampleCount(int srcFreq, int dstFreq, int dstSampleCount)
{
	ASSERT(srcFreq != dstFreq);

	//Resampling formula:
	//    srcSampleCount = dstSampleCount * srcFreq / dstFreq;
	//is not exact due to float->integer truncating. BETTER:
	//    dstSampleCount = requiredSampleCount - (requiredSampleCount/skipIndex);
	//So: srcSampleCount = destSampleCount * skipIndex / (skipIndex - 1);

	//Using resampling formula as in the ConvertSamples function.
	const int skipIndex = srcFreq / (srcFreq - dstFreq);
	return dstSampleCount * skipIndex / (skipIndex - 1) + 1; //+1 - passing VerifyDstSampleCount :)
}
//---------------------------------------------------------------------------

bool VerifyDstSampleCount(int srcFreq, int srcSampleCount, int dstFreq,
	int testDstSampleCount)
{
	const int skipIndex = srcFreq / (srcFreq - dstFreq);
	//const int dstSampleCount = srcSampleCount - (srcSampleCount/skipIndex);

	int skippedSamples = 0;
	for (int i = 0; i < srcSampleCount; i++)
	{
		if ((i % skipIndex) == 0)
			skippedSamples++;
	}

	const int dstSampleCount = srcSampleCount - skippedSamples;
	return (testDstSampleCount == dstSampleCount);
}

/////////////////////////////////////////////////////////////////////////////

SampleBuffer::SampleBuffer(int freq, int chans, float* buffer)
	:frequency(freq)
	,channels(chans)
	,data(buffer)
	,maxSamples(0)
	,curSamples(0)
{
}

/////////////////////////////////////////////////////////////////////////////

class SampleCopier
{
	int m_srcChannels;
	int m_dstChannels;

public:
	SampleCopier(int srcChannels, int dstChannels)
		:m_srcChannels(srcChannels)
		,m_dstChannels(dstChannels)
	{
		ASSERT(m_srcChannels > 0 && m_srcChannels <= 2);
		ASSERT(m_dstChannels > 0 && m_dstChannels <= 2);
	}
	void CopySample(float* srcBuffer, float* dstBuffer)
	{
		if (m_dstChannels == 1) //Mono->Mono; Stereo->Mono;
		{
			*dstBuffer = *srcBuffer;
		}
		else
		if (m_dstChannels == 2) //Mono->Stereo; Stereo->Stereo;
		{
			*dstBuffer = *srcBuffer;
			*(dstBuffer+1) = (m_srcChannels > 1) ? *(srcBuffer+1) : *srcBuffer;
		}
	}
	void CopySample(int index, float* srcBuffer, float* dstBuffer)
	{
		CopySample(index, srcBuffer, index, dstBuffer);
	}
	void CopySample(int sampleIndexSrc, float* srcBuffer, int sampleIndexDst, float* dstBuffer)
	{
		const int srcOffset = sampleIndexSrc*m_srcChannels;
		const int dstOffset = sampleIndexDst*m_dstChannels;
		CopySample(srcBuffer + srcOffset, dstBuffer + dstOffset);
	}
};

/////////////////////////////////////////////////////////////////////////////
/*
int ConvertSamples(float* buffer, int size, int freqIn, int freqOut)
{
	ASSERT(freqIn >= freqOut);
	if (freqIn == freqOut)
		return size; //same data, nothing to convert

	//So, buffer contains samples with frequency freqIn.
	//
	//

}
//---------------------------------------------------------------------------
*/
void ConvertSamples(const SampleBuffer& in, SampleBuffer& out)
{
	ASSERT(in.frequency >= out.frequency);
	ASSERT(in.maxSamples <= out.maxSamples);

	if (in.frequency == out.frequency)
	{
		SampleCopier sc(in.channels, out.channels);
		for (unsigned i = 0; i < in.curSamples; i++)
			sc.CopySample(i, in.data, out.data);

		out.curSamples = in.curSamples;
		return; //just exit, nothing to convert
	}

	//const int resultSamples = int((float(out.frequency)/in.frequency)*in.samples + 0.49);
	//const int skipIndex = in.samples/(in.samples-resultSamples);
	const int skipIndex = in.frequency / (in.frequency - out.frequency);

	int skippedSamples = 0;
	SampleCopier sc(in.channels, out.channels);
	for (unsigned i = 0; i < in.curSamples; i++)
	{
		if ((i % skipIndex) == 0)
			skippedSamples++;
		else
			sc.CopySample(i, in.data, i-skippedSamples, out.data);
	}

	//Finalizing 'out' buffer:
	out.curSamples = in.curSamples - skippedSamples;
}

/////////////////////////////////////////////////////////////////////////////

} //namespace SampleConverter
