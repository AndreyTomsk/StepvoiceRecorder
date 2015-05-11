#include "stdafx.h"
#include "SampleConverter.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	ASSERT(in.channels == out.channels);
	const int sampleSizeBytes = sizeof(float) * in.channels;

	if (in.frequency == out.frequency)
	{
		memcpy(out.data, in.data, in.curSamples * sampleSizeBytes);
		out.curSamples = in.curSamples;
		return; //just exit, nothing to convert
	}

	//const int resultSamples = int((float(out.frequency)/in.frequency)*in.samples + 0.49);
	//const int skipIndex = in.samples/(in.samples-resultSamples);
	const int skipIndex = in.frequency / (in.frequency - out.frequency);

	int skippedSamples = 0;
	for (unsigned i = 0; i < in.curSamples; i++)
	{
		if ((i % skipIndex) == 0)
		{
			skippedSamples++;
			continue;
		}

		const int inOffset = i*in.channels;
		const int outOffset = (i-skippedSamples)*in.channels;
		if (in.channels == 1)
		{
			out.data[outOffset] = in.data[inOffset];
		}
		else
		if (in.channels == 2)
		{
			out.data[outOffset] = in.data[inOffset];
			out.data[outOffset+1] = in.data[inOffset+1];
		}
		else
		{
			memcpy(out.data+outOffset, in.data+inOffset, sampleSizeBytes);
		}
	}

	//Finalizing 'out' buffer:
	out.curSamples = in.curSamples - skippedSamples;
}
//---------------------------------------------------------------------------

