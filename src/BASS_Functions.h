/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#ifndef _BASS_FUNCTIONS_H
#define _BASS_FUNCTIONS_H

/////////////////////////////////////////////////////////////////////////////

namespace Bass
{
	float GetMaxPeakDB(DWORD a_handle);

	bool IsPlaybackDeviceValid(int a_device);
	bool IsRecordingDeviceValid(int a_device);

	CString ErrorGetText();
	void ShowErrorFrom(const CString& bassFunctionName);

} // namespace Bass

/////////////////////////////////////////////////////////////////////////////

#endif //_BASS_FUNCTIONS_H
