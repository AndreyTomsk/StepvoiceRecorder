/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

//////////////////////////////////////////////////////////////////////
//Path - path to registry entry. For example: "File types\\MP3\\Bitrate".
//Here "File types\\MP3" is registry path, "Bitrate" - entry name.

class RegistryConfig
{
public:
	//static void SetRegistryKey(const CString& regPath);

	template<class T>
	static T GetOption(const CString& path, const T& defaultValue);

	template<class T>
	static void SetOption(const CString& path, const T& value);

private:
	RegistryConfig() {}
};
