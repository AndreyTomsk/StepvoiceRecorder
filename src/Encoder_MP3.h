#ifndef ENCODER_MP3_H
#define ENCODER_MP3_H
#pragma once

#include <BladeMP3EncDll.h>
#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////
//Uses "lame_enc.dll" library for mp3 encoding. Expects float buffers.

class CEncoder_MP3 : public Filter
{
public:
	//Throws CString exceptions, in case of "lame_enc.dll" not found or not correct.
	CEncoder_MP3(int bitrate, int frequency, int channels);
	virtual ~CEncoder_MP3();

	//This method recommended to call after mp3 file is finished (closed).
	void WriteVBRHeader(const CString& filePath);

	bool EncodeChunkFloat(float* bufIn, int bufInSize, char* bufOut, int& bufOutSize);

private:
	void LoadLameLibrary();
	void FreeLameLibrary();

	void InitEncoder(int bitrate, int frequency, int channels);
	void CloseEncoder();

	virtual bool ProcessData(void* buffer, DWORD lengthBytes); //overridden


	HINSTANCE  m_hDll;
	BE_CONFIG  m_beConfig;	// stores options for encoder
	HBE_STREAM m_hbeStream;	// encoding stream handle

	float* m_chunkBufFloat_l;
	float* m_chunkBufFloat_r;
	int    m_chunkBufFloatSize;
};

/////////////////////////////////////////////////////////////////////////////
#endif
