//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "Config.h"
#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfig::CConfig()
{
	loadConfig();
}

CConfig::~CConfig()
{
	saveConfig();
	SetAutoRun( conf_dial_gen.bAutoRun != 0 );
}

//////////////////////////////////////////////////////////////////////
// Register read/write functions
//////////////////////////////////////////////////////////////////////

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
	conf_prog.nPlayDevice     = App->GetProfileInt( "General", "Playback device", -1);
	conf_prog.nPlayVolume     = App->GetProfileInt( "General", "Playback volume", 10000);
	conf_prog.nRecDevice      = App->GetProfileInt( "General", "Recording device", -1);
	conf_prog.nRecLineID      = App->GetProfileInt( "General", "Recording line", 0);

	conf_dial_gen.nLoader	= App->GetProfileInt( "General", "Loader", 0 );
	conf_dial_gen.bMinimized= App->GetProfileInt( "General", "Minimized", 0 );
	conf_dial_gen.bAutoRun	= App->GetProfileInt( "General", "AutoRun", 0 );
	conf_dial_gen.bToolTips = App->GetProfileInt( "General", "Tool Tips", 1 );
	conf_dial_gen.bTrayIcon	= App->GetProfileInt( "General", "Show icon in tray", 1 );
	conf_dial_gen.bTrayMin	= App->GetProfileInt( "General", "Minimize to tray", 1 );
	conf_dial_gen.bMInstances = App->GetProfileInt( "General", "Multiple instances", 0 );

	conf_dial_mp3.nBitrate	= App->GetProfileInt( "File types\\MP3", "Bitrate",	128 );
	conf_dial_mp3.nFreq		= App->GetProfileInt( "File types\\MP3", "Freq", 44100 );
	conf_dial_mp3.nStereo	= App->GetProfileInt( "File types\\MP3", "Stereo", 1 );
	conf_dial_mp3.bAutoFreq	= App->GetProfileInt( "File types\\MP3", "AutoFreq", 1 );

	conf_dial_sh2.bIsEnabled= App->GetProfileInt( "Tools\\Scheduler", "Enable",	0 );
	conf_dial_sh2.t_start.h	= App->GetProfileInt( "Tools\\Scheduler", "StartH",	0 );
	conf_dial_sh2.t_start.m	= App->GetProfileInt( "Tools\\Scheduler", "StartM",	0 );
	conf_dial_sh2.t_start.s	= App->GetProfileInt( "Tools\\Scheduler", "StartS",	0 );
	conf_dial_sh2.t_stop.h	= App->GetProfileInt( "Tools\\Scheduler", "StopH", 1 );
	conf_dial_sh2.t_stop.m	= App->GetProfileInt( "Tools\\Scheduler", "StopM", 30);
	conf_dial_sh2.t_stop.s	= App->GetProfileInt( "Tools\\Scheduler", "StopS", 0 );
	conf_dial_sh2.t_rec.h	= App->GetProfileInt( "Tools\\Scheduler", "RecH", 0 );
	conf_dial_sh2.t_rec.m	= App->GetProfileInt( "Tools\\Scheduler", "RecM", 5 );
	conf_dial_sh2.t_rec.s	= App->GetProfileInt( "Tools\\Scheduler", "RecS", 0 );
	conf_dial_sh2.bSchedStart = App->GetProfileInt( "Tools\\Scheduler", "SchedStart", 0 );
	conf_dial_sh2.bRunExternal= App->GetProfileInt( "Tools\\Scheduler", "RunExternal", 0 );
	conf_dial_sh2.nStopByID	  = App->GetProfileInt( "Tools\\Scheduler", "StopByID",	0 );
	conf_dial_sh2.strFileName = App->GetProfileString( "Tools\\Scheduler", "FileToRun", "" );
	conf_dial_sh2.action.shutDown = App->GetProfileInt( "Tools\\Scheduler", "ActShutDown", 0 );
	conf_dial_sh2.action.closeAll = App->GetProfileInt( "Tools\\Scheduler", "ActCloseAll", 0 );

	conf_dial_vas.bEnable	= App->GetProfileInt( "Tools\\VAS", "Enable", 0 );
	conf_dial_vas.nThreshold= App->GetProfileInt( "Tools\\VAS", "Threshold", -30 );
	conf_dial_vas.nWaitTime = App->GetProfileInt( "Tools\\VAS", "WaitTime", 2000 );
	conf_dial_vas.nAction	= App->GetProfileInt( "Tools\\VAS", "Action", 0 );

	conf_dial_an.strAutoName = App->GetProfileString( "Autoname", "Current template", "%b%d_%H%M" );
	conf_dial_an.strTDefault = App->GetProfileString( "Autoname", "Default template", "%b%d_%H%M" );
}

//====================================================================
bool CConfig::RegisterWrite()
{
	CWinApp* App = AfxGetApp();

	App->WriteProfileInt( "General", "Always on Top",	conf_prog.bAlwaysOnTop );
	App->WriteProfileInt( "General", "Easy Move",		conf_prog.bEasyMove );
	App->WriteProfileInt( "General", "Xcoord",			conf_prog.nXcoord );
	App->WriteProfileInt( "General", "Ycoord",			conf_prog.nYcoord );
	App->WriteProfileInt( "General", "DialogIndex",		conf_prog.nDialogIndex );
	App->WriteProfileString( "General", "LastFileName",	conf_prog.strLastFileName );
	App->WriteProfileString( "General", "LastFilePath",	conf_prog.strLastFilePath );
	App->WriteProfileInt( "General", "Graph Type",		conf_prog.nGraphType );
	App->WriteProfileInt( "General", "Show max peaks",	conf_prog.bGraphMaxpeaks);
	App->WriteProfileInt( "General", "Sound Monitor",	conf_prog.bMonitoring );
	App->WriteProfileInt( "General", "Playback device",	conf_prog.nPlayDevice );
	App->WriteProfileInt( "General", "Playback volume",	conf_prog.nPlayVolume );
	App->WriteProfileInt( "General", "Recording device",	conf_prog.nRecDevice );
	App->WriteProfileInt( "General", "Recording line",	conf_prog.nRecLineID );

	App->WriteProfileInt( "General", "Loader",			conf_dial_gen.nLoader );
	App->WriteProfileInt( "General", "Minimized",		conf_dial_gen.bMinimized );
	App->WriteProfileInt( "General", "AutoRun",			conf_dial_gen.bAutoRun );
	App->WriteProfileInt( "General", "Tool Tips",		conf_dial_gen.bToolTips );
	App->WriteProfileInt( "General", "Show icon in tray",  conf_dial_gen.bTrayIcon );
	App->WriteProfileInt( "General", "Minimize to tray",   conf_dial_gen.bTrayMin );
	App->WriteProfileInt( "General", "Multiple instances", conf_dial_gen.bMInstances );

	App->WriteProfileInt( "File types\\MP3", "Bitrate",	conf_dial_mp3.nBitrate );
	App->WriteProfileInt( "File types\\MP3", "Freq",	conf_dial_mp3.nFreq );
	App->WriteProfileInt( "File types\\MP3", "Stereo",	conf_dial_mp3.nStereo );
	App->WriteProfileInt( "File types\\MP3", "AutoFreq",conf_dial_mp3.bAutoFreq );

	App->WriteProfileInt( "Tools\\Scheduler", "Enable",		 conf_dial_sh2.bIsEnabled );
	App->WriteProfileInt( "Tools\\Scheduler", "StartH",		 conf_dial_sh2.t_start.h );
	App->WriteProfileInt( "Tools\\Scheduler", "StartM",		 conf_dial_sh2.t_start.m );
	App->WriteProfileInt( "Tools\\Scheduler", "StartS",		 conf_dial_sh2.t_start.s );
	App->WriteProfileInt( "Tools\\Scheduler", "StopH",		 conf_dial_sh2.t_stop.h );
	App->WriteProfileInt( "Tools\\Scheduler", "StopM",		 conf_dial_sh2.t_stop.m );
	App->WriteProfileInt( "Tools\\Scheduler", "StopS",		 conf_dial_sh2.t_stop.s );
	App->WriteProfileInt( "Tools\\Scheduler", "RecH",		 conf_dial_sh2.t_rec.h );
	App->WriteProfileInt( "Tools\\Scheduler", "RecM",		 conf_dial_sh2.t_rec.m );
	App->WriteProfileInt( "Tools\\Scheduler", "RecS",		 conf_dial_sh2.t_rec.s );
	App->WriteProfileInt( "Tools\\Scheduler", "SchedStart",	 conf_dial_sh2.bSchedStart );
	App->WriteProfileInt( "Tools\\Scheduler", "RunExternal", conf_dial_sh2.bRunExternal );
	App->WriteProfileString( "Tools\\Scheduler","FileToRun", conf_dial_sh2.strFileName );
	App->WriteProfileInt( "Tools\\Scheduler", "StopByID",	 conf_dial_sh2.nStopByID );
	App->WriteProfileInt( "Tools\\Scheduler", "ActShutDown", conf_dial_sh2.action.shutDown );
	App->WriteProfileInt( "Tools\\Scheduler", "ActCloseAll", conf_dial_sh2.action.closeAll );

	App->WriteProfileInt( "Tools\\VAS", "Enable",	 conf_dial_vas.bEnable );
	App->WriteProfileInt( "Tools\\VAS", "Threshold", conf_dial_vas.nThreshold );
	App->WriteProfileInt( "Tools\\VAS", "WaitTime",  conf_dial_vas.nWaitTime );
	App->WriteProfileInt( "Tools\\VAS", "Action",    conf_dial_vas.nAction );
	
	App->WriteProfileString( "Autoname", "Current template", conf_dial_an.strAutoName );
	int b = App->WriteProfileString( "Autoname", "Default template", conf_dial_an.strTDefault );
	//int b = App->WriteProfileString( "Autoname", "Default template", "%b%d_%H%M" );
	//int b = App->WriteProfileString( "Autoname", "Default template", "%Y%m%d_%H%M" );

	return ( b != 0 );
}

//====================================================================
bool CConfig::SetAutoRun( bool bSet )
{
	HKEY hKey;
	const char szAppName[]	  = "SvRecorder";
	const char szRegRunPath[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

	LONG lResult = RegOpenKeyEx( HKEY_CURRENT_USER, szRegRunPath, 0,
		KEY_ALL_ACCESS,	&hKey);

	if( lResult != ERROR_SUCCESS )
		return false;

	if( bSet ) {
		char szFullAppName[500];
		GetModuleFileName( NULL, szFullAppName, 500 );
		RegSetValueEx( hKey, szAppName, 0, REG_SZ,
			reinterpret_cast<const unsigned char *>(szFullAppName),
			strlen( szFullAppName ) + 1 );
	}
	else
		RegDeleteValue( hKey, szAppName );

	RegCloseKey( hKey );
	return true;
}

//////////////////////////////////////////////////////////////////////
