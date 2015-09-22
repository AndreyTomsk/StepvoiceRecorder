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

	bool loadConfig();
	bool saveConfig();

	CONF_PROG*      GetConfProg()    { return &conf_prog; }
	CONF_DIAL_GEN*	GetConfDialGen() { return &conf_dial_gen; }
	CONF_DIAL_MP3*	GetConfDialMp3() { return &conf_dial_mp3; }
	CONF_DIAL_AN*	GetConfDialAN()  { return &conf_dial_an; }

private:
	void RegisterRead();
	void ValidateRead();
	bool RegisterWrite();
	//bool SetAutoRun( bool bSet );

private:
	CONF_PROG		conf_prog;
	CONF_DIAL_GEN	conf_dial_gen;
	CONF_DIAL_MP3	conf_dial_mp3;
	CONF_DIAL_AN	conf_dial_an;
};

//////////////////////////////////////////////////////////////////////
//Path - path to registry entry. For example: "File types\\MP3\\Bitrate".
//Here "File types\\MP3" is registry path, "Bitrate" - entry name.

class RegistryConfig
{
public:
	static void SetRegistryKey(const CString& regPath);

	template<class T>
	static T GetOption(const CString& path, const T& defaultValue);

	template<class T>
	static void SetOption(const CString& path, const T& value);

private:
	RegistryConfig() {}
};

//////////////////////////////////////////////////////////////////////
#endif
