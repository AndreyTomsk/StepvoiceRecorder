; -- SvRec.iss --
; Stepvoice Recorder installation script
; ===========================================

#define User_AppName    "Stepvoice Recorder"
#define User_AppExeName "SvRec.exe"
#define MyCompanyUrl    "www.stepvoice.com"
#define MyCompanyName   "Andrey Firsov"

#define FullAppVersion GetFileVersion(AddBackslash(SourcePath) + "..\bin\" + User_AppExeName)
#define User_AppVersion Copy(FullAppVersion, 1, 5)
#define User_OutputFile "svrec" + Copy(User_AppVersion, 1, 1) + Copy(User_AppVersion, 3, 1)

;Creating StepvoiceRecorderUpdate.xml. Placed together with installer on server download page.
#expr Exec("make_update_xml.bat", FullAppVersion + ' ' + User_OutputFile + '.exe')

[Setup]
AppName={#User_AppName}
AppID={#User_AppName}
AppCopyright=Copyright (C) 2004-2016 {#MyCompanyName}
AppVersion={#FullAppVersion}
AppVerName={#User_AppName} {#User_AppVersion}
DefaultDirName={pf}\{#User_AppName}
DefaultGroupName={#User_AppName}
DirExistsWarning=no
WizardImageFile=WizModernImage-IS.bmp
WizardSmallImageFile=WizModernSmallImage-IS.bmp
LicenseFile=..\license.txt
Compression=lzma/max
SolidCompression=yes
DisableWelcomePage=no

OutputBaseFilename={#User_OutputFile}
#ifndef SignSkip
SignTool=Standard /d $qStepvoice Recorder Setup$q /du $qhttp://stepvoice.com$q $f
#endif


;Installer-related options (Uninstall data)
AppPublisher={#MyCompanyName}
AppPublisherURL={#MyCompanyUrl}
AppSupportURL={#MyCompanyUrl}
UninstallDisplayIcon={app}\SvRec.exe

[Registry]
Root: HKLM; Subkey: "Software\Stepvoice Software"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\Stepvoice Software\SvRec"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Stepvoice Software"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\Stepvoice Software\SvRec"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Stepvoice Software\SvRec\General"; ValueType: string; ValueName: "LastFilePath"; ValueData: "{userdesktop}"

[Files]
Source: ..\bin\{#User_AppExeName}; DestDir: {app}; Flags: ignoreversion
Source: ..\bin\bass.dll;           DestDir: {app}; Flags: ignoreversion
Source: ..\bin\basswasapi.dll;     DestDir: {app}; Flags: ignoreversion
Source: ..\bin\lame_enc.dll;       DestDir: {app}; Flags: ignoreversion
Source: ..\bin\ResetOptions.reg;   DestDir: {app}; Flags: ignoreversion
Source: ..\doc\Help\SvRec.chm;     DestDir: {app}; Flags: ignoreversion
Source: Stepvoice Recorder Home.url; DestDir: {app}; Flags: ignoreversion

[Dirs]
Name: "{commonappdata}\Stepvoice"

[Icons]
Name: {group}\Stepvoice Recorder;             FileName: {app}\{#User_AppExeName}; WorkingDir: {app}
Name: {group}\Stepvoice Recorder Help;        FileName: {app}\SvRec.chm; WorkingDir: {app}
Name: {group}\Stepvoice Recorder Home;        Filename: {app}\Stepvoice Recorder Home.url;
Name: {group}\Uninstall;                      FileName: {uninstallexe}

Name: {userdesktop}\Stepvoice Recorder;       FileName: {app}\{#User_AppExeName}; WorkingDir: {app}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Stepvoice Recorder; FileName: {app}\{#User_AppExeName}; WorkingDir: {app}; Tasks: quicklaunchicon

[UninstallDelete]
Type: filesandordirs; Name: "{commonappdata}\Stepvoice"

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:
Name: quicklaunchicon; Description: Create a &Quick Launch icon; GroupDescription: Additional icons:; Flags: unchecked

[Run]
FileName: {app}\{#User_AppExeName}; Description: Launch Stepvoice Recorder; Flags: postinstall nowait skipifsilent unchecked

[Code]
var
  regInfo: String; // will contain a user registration key
  g_version_short: String;
  
// Before INSTALL check if the program already running
function InitializeSetup(): Boolean;
begin
  // Exctracting two version numbers
  g_version_short := '{#User_AppVersion}';
  g_version_short := Copy(g_version_short, 0, 4);
  StringChangeEx(g_version_short, '.', '', True);
  
  if CheckForMutexes('SVREC-169A0B91-77B7-4533-9C25-59FCB08FCD614') then
  begin
    MsgBox('Stepvoice Recorder is running, please close it and restart the setup application.',
      mbError, MB_OK);
    Result := false;
  end
  else
    Result := true;
end;

function GetVersionShort(Param: String): String;
begin
  Result := g_version_short;
end;

//
function IsUpgrade(): Boolean;
var
   sPrevPath: String;
begin
  sPrevPath := '';
  if not RegQueryStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#SetupSetting("AppID")}_is1', 'UninstallString', sPrevpath) then
    RegQueryStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#SetupSetting("AppID")}_is1', 'UninstallString', sPrevpath);
  Result := (sPrevPath <> '');
end;

//
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := False;
  if IsUpgrade() then
  begin
    if PageID = wpSelectDir then Result := True;
    if PageID = wpSelectProgramGroup then Result := True;
  end;
end;

// Before UNINSTALL check if the program already running
function InitializeUninstall(): Boolean;
begin
  if CheckForMutexes('SVREC-169A0B91-77B7-4533-9C25-59FCB08FCD614') then
  begin
    MsgBox('Stepvoice Recorder is running, please close it and restart the uninstall process.',
      mbError, MB_OK);
    Result := false;
  end
  else
    Result := true;
end;
