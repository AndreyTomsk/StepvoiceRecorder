#include "stdafx.h"
#include <Mmreg.h> //for WAVE_FORMAT_IEEE_FLOAT
#include "FilterFileWriterWAV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

//#pragma pack(show)
#pragma pack(push, 1)
struct WAVEHEADER
{
	char   riff_tag[4];
	UINT32 riff_length;
	char   wave_tag[4];
	char   fmt_tag[4];
	UINT32 fmt_length;
	UINT16 audio_format;
	UINT16 num_channels;
	UINT32 sample_rate;
	UINT32 byte_rate;
	UINT16 block_align;
	UINT16 bits_per_sample;
	char   data_tag[4];
	UINT32 data_length;
};
#pragma pack(pop)

/////////////////////////////////////////////////////////////////////////////

FileWriterWAV::FileWriterWAV(const CString& fileName, int frequency, int channels)
	:FileWriter(fileName)
{
	WriteWaveHeaderFloatPCM(frequency, channels);
}
//---------------------------------------------------------------------------

FileWriterWAV::~FileWriterWAV()
{
	ASSERT(m_outputFile.m_hFile != CFile::hFileNull);
	FinishWaveHeaderBeforeClosing();
}
//---------------------------------------------------------------------------

void FileWriterWAV::WriteWaveHeaderFloatPCM(int frequency, int channels)
{
	 //Resulution of a sample point. 32 for WAVE_FORMAT_IEEE_FLOAT, according to MSDN.
	const int BITS_PER_SAMPLE = 8*sizeof(float);

	struct WAVEHEADER header;
	memcpy(header.riff_tag, "RIFF", 4);
	header.riff_length = 0; //MUST be filled after recording is complete
	memcpy(header.wave_tag, "WAVE", 4);
	memcpy(header.fmt_tag, "fmt ", 4);
	header.fmt_length = 16; //number of bytes in "fmt " chunk (size after fmt_length, until data_tag, 16 for PCM)
	header.audio_format = WAVE_FORMAT_IEEE_FLOAT;
	header.num_channels = (UINT16)channels;
	header.sample_rate = frequency;
	header.byte_rate = frequency*channels*BITS_PER_SAMPLE/8;
	header.block_align = (UINT16)channels*BITS_PER_SAMPLE/8;
	header.bits_per_sample = (UINT16)BITS_PER_SAMPLE;
	memcpy(header.data_tag, "data", 4);
	header.data_length = 0; //MUST be filled after recording is complete

	m_outputFile.Write(&header, sizeof(header));
}
//---------------------------------------------------------------------------

void FileWriterWAV::FinishWaveHeaderBeforeClosing()
{
	const ULONGLONG fileLength = m_outputFile.GetLength();
	ASSERT(fileLength < _UI32_MAX);

	//Writing header.data_length
	const UINT32 dataLength = (UINT32)fileLength - sizeof(WAVEHEADER);
	m_outputFile.Seek(sizeof(WAVEHEADER)-sizeof(UINT32), CFile::begin);
	m_outputFile.Write(&dataLength, sizeof(dataLength));

	//Writing header.riff_length
	//Remaining length, excluding header's riff_tag and riff_length (-8 bytes).
	const UINT32 riffLength = (UINT32)fileLength - 8;
	m_outputFile.Seek(4, CFile::begin); //seek to WAVEHEADER.riff_length
	m_outputFile.Write(&riffLength, sizeof(riffLength));
}
//---------------------------------------------------------------------------
