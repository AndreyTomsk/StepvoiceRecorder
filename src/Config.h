//////////////////////////////////////////////////////////////////////
#ifndef CONFIG_H
#define CONFIG_H

#include "struct.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
class CConfig  
{
public:
	CConfig();
	~CConfig();

	bool loadConfig() { RegisterRead(); return true; }
	bool saveConfig() { return RegisterWrite(); }

	CONF_PROG*		GetConfProg()    { return &conf_prog; }
	CONF_DIAL_GEN*	GetConfDialGen() { return &conf_dial_gen; }
	CONF_DIAL_MP3*	GetConfDialMp3() { return &conf_dial_mp3; }
	CONF_DIAL_SH2*	GetConfDialSH2() { return &conf_dial_sh2; }
	CONF_DIAL_VAS*	GetConfDialVAS() { return &conf_dial_vas; }
	CONF_DIAL_AN*	GetConfDialAN()  { return &conf_dial_an; }

private:
	void RegisterRead();
	bool RegisterWrite();
	bool SetAutoRun( bool bSet );

private:
	CONF_PROG		conf_prog;
	CONF_DIAL_GEN	conf_dial_gen;
	CONF_DIAL_MP3	conf_dial_mp3;
	CONF_DIAL_SH2	conf_dial_sh2;
	CONF_DIAL_VAS	conf_dial_vas;
	CONF_DIAL_AN	conf_dial_an;
};

//////////////////////////////////////////////////////////////////////
#endif
