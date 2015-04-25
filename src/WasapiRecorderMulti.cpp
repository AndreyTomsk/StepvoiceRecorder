#include "stdafx.h"
#include <bassmix.h>
#include "WasapiRecorderMulti.h"
#include "WasapiRecorderStream.h"
#include "WasapiHelpers.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiRecorderMulti::CWasapiRecorderMulti(
	WasapiHelpers::DevicesArray devices, DWORD freq, DWORD chans)
{
}
//---------------------------------------------------------------------------

CWasapiRecorderMulti::~CWasapiRecorderMulti()
{
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetActualFrequency() const
{
	//return m_actualFreq;
	return 0;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetActualChannelCount() const
{
	//return m_actualChans;
	return 0;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Start()
{
	return FALSE;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Pause()
{
	return FALSE;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Stop()
{
	return FALSE;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetVolume() const
{
	return 0;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::SetVolume(float volume)
{
	return FALSE;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetPeakLevel(int channel) const
{
	return 0;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetData(void* buffer, DWORD lengthBytes) const
{
	return 0;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetChannelData(int channel, float* buffer, int bufferSize)
{
	return 0;
}
//---------------------------------------------------------------------------
/*
bool CWasapiRecorderMulti::ProcessData(void* buffer, DWORD lengthBytes)
{
	//Just using base implementation: it sends data to next filter in chain.
	//return Filter::ProcessData(buffer, lengthBytes);
	return true;
}
//---------------------------------------------------------------------------
*/