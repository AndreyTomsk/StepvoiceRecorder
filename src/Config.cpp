#include "stdafx.h"
#include "Config.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//NOTE: kept for options history.
/*
void CConfig::RegisterRead()
{
	CWinApp* App = AfxGetApp();

	conf_prog.bAlwaysOnTop    = App->GetProfileInt( "General", "Always on Top", 0 );
	conf_prog.bEasyMove       = App->GetProfileInt( "General", "Easy Move", 1 );
	conf_prog.nXcoord         = App->GetProfileInt( "General", "Xcoord", 300 );
	conf_prog.nYcoord         = App->GetProfileInt( "General", "Ycoord", 200 );
	conf_prog.nDialogIndex    = App->GetProfileInt( "General", "DialogIndex", 0 );
	conf_prog.strLastFileName = App->GetProfileString( "General","LastFileName", "" );
	conf_prog.strLastFilePath = App->GetProfileString( "General","LastFilePath", "C:\\" );
	conf_prog.nGraphType      = App->GetProfileInt( "General", "Graph Type", ID_GRAPH_PEAKMETER );
	conf_prog.bGraphMaxpeaks  = App->GetProfileInt( "General", "Show max peaks", 1);
	conf_prog.bMonitoring     = App->GetProfileInt( "General", "Sound Monitor", 0 );
	conf_prog.nPlayVolume     = App->GetProfileInt( "General", "Playback volume", 10000);
	conf_prog.nRecLineID      = App->GetProfileInt( "General", "Recording line", 0);

	conf_dial_gen.nLoader	= App->GetProfileInt( "General", "Loader", 0 );
	conf_dial_gen.bMinimized= App->GetProfileInt( "General", "Minimized", 0 );
	conf_dial_gen.bAutoRun	= App->GetProfileInt( "General", "AutoRun", 0 );
	conf_dial_gen.bToolTips = App->GetProfileInt( "General", "Tool Tips", 1 );
	conf_dial_gen.bTrayIcon	= App->GetProfileInt( "General", "Show icon in tray", 1 );
	conf_dial_gen.bTrayMin	= App->GetProfileInt( "General", "Minimize to tray", 1 );
	conf_dial_gen.bMInstances = App->GetProfileInt( "General", "Multiple instances", 0 );
	conf_dial_gen.nPlayDevice = App->GetProfileInt( "General", "Playback device", -1);
	conf_dial_gen.nRecDevice  = App->GetProfileInt( "General", "Recording device", -1);

	conf_dial_mp3.nBitrate	= App->GetProfileInt( "File types\\MP3", "Bitrate",	128 );
	conf_dial_mp3.nFreq		= App->GetProfileInt( "File types\\MP3", "Freq", 44100 );
	conf_dial_mp3.nStereo	= App->GetProfileInt( "File types\\MP3", "Stereo", 1 );
	conf_dial_mp3.bAutoFreq	= App->GetProfileInt( "File types\\MP3", "AutoFreq", 1 );

	conf_dial_an.strAutoName = App->GetProfileString( "Autoname", "Current template", "%b%d_%H%M" );
	conf_dial_an.strTDefault = App->GetProfileString( "Autoname", "Default template", "%b%d_%H%M" );
}
//--------------------------------------------------------------------
*/
//void RegistryConfig::SetRegistryKey(const CString& regPath)
//{
//	CMP3_RecorderApp* winApp = dynamic_cast<CMP3_RecorderApp*>(AfxGetApp());
//	winApp->SetRegistryKey(regPath);
//}
//--------------------------------------------------------------------

struct RegistryPlace
{
	CString section;
	CString entry;
};

static RegistryPlace GetRegistryPlace(const CString& initialPath)
{
	RegistryPlace rp;

	const int lastBackslashPos = initialPath.ReverseFind(_T('\\'));
	if (lastBackslashPos == -1)
	{
		rp.entry = initialPath;
	}
	else
	{
		rp.section = initialPath.Left(lastBackslashPos);
		rp.entry   = initialPath.Right(initialPath.GetLength() - lastBackslashPos - 1);
	}

	ASSERT(!rp.entry.IsEmpty());
	return rp;
}
//--------------------------------------------------------------------

template<>
int RegistryConfig::GetOption<int>(const CString& initialPath, const int& defaultValue)
{
	const RegistryPlace rp = GetRegistryPlace(initialPath);
	return AfxGetApp()->GetProfileInt(rp.section, rp.entry, defaultValue);
}
//--------------------------------------------------------------------

template<>
bool RegistryConfig::GetOption<bool>(const CString& initialPath, const bool& defaultValue)
{
	return GetOption<int>(initialPath, defaultValue ? 1 : 0) ? true : false;
}
//--------------------------------------------------------------------

template<>
CString RegistryConfig::GetOption<CString>(const CString& initialPath, const CString& defaultValue)
{
	const RegistryPlace rp = GetRegistryPlace(initialPath);
	return AfxGetApp()->GetProfileString(rp.section, rp.entry, defaultValue);
}
//--------------------------------------------------------------------

template<>
void RegistryConfig::SetOption<int>(const CString& initialPath, const int& value)
{
	const RegistryPlace rp = GetRegistryPlace(initialPath);
	AfxGetApp()->WriteProfileInt(rp.section, rp.entry, value);
}
//--------------------------------------------------------------------

template<>
void RegistryConfig::SetOption<CString>(const CString& initialPath, const CString& value)
{
	const RegistryPlace rp = GetRegistryPlace(initialPath);
	AfxGetApp()->WriteProfileString(rp.section, rp.entry, value);
}
//--------------------------------------------------------------------

template<>
void RegistryConfig::SetOption<long>(const CString& initialPath, const long& value)
{
	SetOption<int>(initialPath, value);
}
//--------------------------------------------------------------------

template<>
void RegistryConfig::SetOption<bool>(const CString& initialPath, const bool& value)
{
	SetOption<int>(initialPath, value ? 1 : 0);
}
//--------------------------------------------------------------------
