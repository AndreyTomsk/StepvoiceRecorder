; -- SvRec.iss --
; StepVoice Recorder installation script
; ===========================================

#define User_AppName    "StepVoice Recorder"
#define User_AppExeName "SvRec.exe"

#define User_AppVersion GetFileVersion(AddBackslash(SourcePath) + "..\bin\" + User_AppExeName)
#define User_OutputFile "svrec" + Copy(User_AppVersion, 1, 1) + Copy(User_AppVersion, 3, 1)

[Setup]
AppName={#User_AppName}
AppID={#User_AppName}
AppVerName={#User_AppName} {#User_AppVersion}
DefaultDirName={pf}\{#User_AppName}
DefaultGroupName={#User_AppName}
UninstallDisplayIcon={app}\SvRec.exe
DirExistsWarning=no
WizardImageFile=WizModernImage-IS.bmp
WizardSmallImageFile=WizModernSmallImage-IS.bmp
LicenseFile=.\license.txt
Compression=lzma/max
SolidCompression=yes
PrivilegesRequired=none

OutputBaseFilename={#User_OutputFile}


[Registry]
Root: HKLM; Subkey: "Software\StepVoice Software"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\StepVoice Software\SvRec"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\StepVoice Software"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\StepVoice Software\SvRec"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\StepVoice Software\SvRec\General"; ValueType: string; ValueName: "LastFilePath"; ValueData: "{userdesktop}"
;Root: HKLM; Subkey: "Software\My Company\My Program\Settings"; ValueType: string; ValueName: "Path"; ValueData: "{app}"


[Files]
Source: history.txt;               DestDir: {app}; Flags: ignoreversion
Source: license.txt;               DestDir: {app}; Flags: ignoreversion
Source: file_id.diz;               DestDir: {app}; Flags: ignoreversion
Source: Stepvoice Recorder Home.url; DestDir: {app}; Flags: ignoreversion
Source: order.url;                 DestDir: {app}; Flags: ignoreversion

Source: ..\bin\{#User_AppExeName}; DestDir: {app}; Flags: ignoreversion
Source: ..\bin\lame_enc.dll;       DestDir: {app}; Flags: ignoreversion
Source: ..\doc\Help\SvRec.chm;     DestDir: {app}; Flags: ignoreversion
Source: ..\..\bass\bass.dll;       DestDir: {app}; Flags: ignoreversion

[Icons]
;Name: {group}\Order Online;                   Filename: {app}\order.url; WorkingDir: {app}
Name: {group}\StepVoice Recorder;             FileName: {app}\{#User_AppExeName}; WorkingDir: {app}
Name: {group}\StepVoice Recorder Help;        FileName: {app}\SvRec.chm; WorkingDir: {app}
Name: {group}\StepVoice Recorder Home;        Filename: {app}\StepVoice Recorder Home.url;
Name: {group}\Uninstall;                      FileName: {uninstallexe}

Name: {userdesktop}\StepVoice Recorder;       FileName: {app}\{#User_AppExeName}; WorkingDir: {app}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\StepVoice Recorder; FileName: {app}\{#User_AppExeName}; WorkingDir: {app}; Tasks: quicklaunchicon

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:
Name: quicklaunchicon; Description: Create a &Quick Launch icon; GroupDescription: Additional icons:; Flags: unchecked

[Run]
; The line below is used only for special versions.
Filename: {app}\{#User_AppExeName}; Parameters: "/register"
FileName: {app}\{#User_AppExeName}; Description: Launch StepVoice Recorder; Flags: postinstall nowait skipifsilent unchecked

[_ISTool]
Use7zip=false

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
    MsgBox('StepVoice Recorder is running, please close it and run again setup.',
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
    MsgBox('StepVoice Recorder is running, please close it and run again uninstall.',
      mbError, MB_OK);
    Result := false;
  end
  else
    Result := true;
end;

// While uninstall aks a user if he wants to keep his registration info
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  saveRegInfo: Boolean;
begin
  case CurUninstallStep of
    usUninstall: // pre-uninstall tasks
      begin
        regInfo := '';
        if RegValueExists(HKEY_CURRENT_USER, 'Software\StepVoice Software\SvRec', 'Key') then
        begin
          Beep();
          saveRegInfo := MsgBox('Would you like to keep your registration info?', mbConfirmation, MB_YESNO) = idYes;
          if saveRegInfo then
            RegQueryStringValue(HKEY_CURRENT_USER, 'Software\StepVoice Software\SvRec', 'Key', regInfo);
        end
      end;
    usPostUninstall: // post-uninstall tasks
      begin
        if regInfo <> '' then
          RegWriteStringValue(HKEY_CURRENT_USER, 'Software\StepVoice Software\SvRec', 'Key', regInfo);
      end;
  end;
end;

//
