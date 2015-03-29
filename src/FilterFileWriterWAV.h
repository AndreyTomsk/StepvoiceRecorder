#ifndef FILTER_FILE_WRITER_WAV_H
#define FILTER_FILE_WRITER_WAV_H
#pragma once

#include "FilterFileWriter.h"

/////////////////////////////////////////////////////////////////////////////
//Writing into 32bit float PCM WAV file.

class FileWriterWAV : public FileWriter
{
public:
	FileWriterWAV(const CString& fileName, int frequency, int channels);
	virtual ~FileWriterWAV();

private:
	void WriteWaveHeaderFloatPCM(int frequency, int channels);
	void FinishWaveHeaderBeforeClosing();
};

/////////////////////////////////////////////////////////////////////////////
#endif
