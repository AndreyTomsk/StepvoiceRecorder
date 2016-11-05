/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include <bass.h>
#include <basswasapi.h>
#include <cmath>
#include "Bass_Functions.h"

/////////////////////////////////////////////////////////////////////////////

namespace Bass
{
//---------------------------------------------------------------------------

float GetMaxPeakDB(DWORD a_handle)
{
	DWORD l_ch_level = BASS_ChannelGetLevel(a_handle); // 0..32768
	ASSERT(l_ch_level != -1);

	DWORD l_level = max(LOWORD(l_ch_level), HIWORD(l_ch_level));
	return 20 * std::log10(float(l_level) / 32768);
}
//---------------------------------------------------------------------------

bool IsPlaybackDeviceValid(int a_device)
{
	BASS_DEVICEINFO info;
	BOOL result = BASS_GetDeviceInfo(a_device + 1, &info);
	return result && (info.flags&BASS_DEVICE_ENABLED);
}
//---------------------------------------------------------------------------

bool IsRecordingDeviceValid(int a_device)
{
	BASS_DEVICEINFO info;
	BOOL result = BASS_RecordGetDeviceInfo(a_device + 1, &info);
	return result && (info.flags&BASS_DEVICE_ENABLED);
}
//---------------------------------------------------------------------------

CString ErrorGetText()
{
	const int errorCode = BASS_ErrorGetCode();
	switch (errorCode)
	{
		case BASS_OK		: return L"all is OK";
		case BASS_ERROR_MEM	: return L"memory error";
		case BASS_ERROR_FILEOPEN: return L"can't open the file";
		case BASS_ERROR_DRIVER	: return L"can't find a free/valid driver";
		case BASS_ERROR_BUFLOST : return L"the sample buffer was lost";
		case BASS_ERROR_HANDLE	: return L"invalid handle";
		case BASS_ERROR_FORMAT	: return L"unsupported sample format";
		case BASS_ERROR_POSITION: return L"invalid position";
		case BASS_ERROR_INIT	: return L"BASS_Init has not been successfully called";
		case BASS_ERROR_START	: return L"BASS_Start has not been successfully called";
		case BASS_ERROR_ALREADY	: return L"already initialized/paused/etc.";
		case BASS_ERROR_NOCHAN	: return L"can't get a free channel";
		case BASS_ERROR_ILLTYPE	: return L"an illegal type was specified";
		case BASS_ERROR_ILLPARAM: return L"an illegal parameter was specified";
		case BASS_ERROR_NO3D	: return L"no 3D support";
		case BASS_ERROR_NOEAX	: return L"no EAX support";
		case BASS_ERROR_DEVICE	: return L"illegal device number";
		case BASS_ERROR_NOPLAY	: return L"not playing";
		case BASS_ERROR_FREQ	: return L"illegal sample rate";
		case BASS_ERROR_NOTFILE	: return L"the stream is not a file stream";
		case BASS_ERROR_NOHW	: return L"no hardware voices available";
		case BASS_ERROR_EMPTY	: return L"the MOD music has no sequence data";
		case BASS_ERROR_NONET	: return L"no internet connection could be opened";
		case BASS_ERROR_CREATE	: return L"couldn't create the file";
		case BASS_ERROR_NOFX	: return L"effects are not available";
		case BASS_ERROR_NOTAVAIL: return L"requested data is not available";
		case BASS_ERROR_DECODE	: return L"the channel is/isn't a \"decoding channel\"";
		case BASS_ERROR_DX	: return L"a sufficient DirectX version is not installed";
		case BASS_ERROR_TIMEOUT	: return L"connection timedout";
		case BASS_ERROR_FILEFORM: return L"unsupported file format";
		case BASS_ERROR_SPEAKER	: return L"unavailable speaker";
		case BASS_ERROR_VERSION	: return L"invalid BASS version (used by add-ons)";
		case BASS_ERROR_CODEC	: return L"codec is not available/supported";
		case BASS_ERROR_ENDED	: return L"the channel/file has ended";
		case BASS_ERROR_BUSY	: return L"the device is busy";
		default			: return L"unknown problem";
	}
}
//---------------------------------------------------------------------------

void ShowErrorFrom(const CString& bassFunctionName)
{
	CString msgError;
	msgError.Format(_T("%s error:\r\n'%s'"), bassFunctionName, Bass::ErrorGetText());
	AfxMessageBox(msgError, MB_OK);
}

/////////////////////////////////////////////////////////////////////////////

} // namespace Bass
