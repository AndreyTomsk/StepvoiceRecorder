; -- SvRec.iss --
; StepVoice Recorder installation script
; ===========================================

#define AppName "StepVoice Recorder"
#define AppVersion GetStringFileInfo(AddBackslash(SourcePath) + "..\bin\SvRec.exe", FILE_VERSION)

[Setup]
AppName={#AppName}
AppID={#AppName}
AppVerName={#AppName} {#AppVersion}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\SvRec.exe
DirExistsWarning=no
WizardImageFile=WizModernImage-IS.bmp
WizardSmallImageFile=WizModernSmallImage-IS.bmp
OutputBaseFilename=svrec16
LicenseFile=.\license.txt
Compression=lzma/max
SolidCompression=yes
PrivilegesRequired=none


[Registry]
Root: HKLM; Subkey: "Software\StepVoice Software"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\StepVoice Software\SvRec"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\StepVoice Software"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\StepVoice Software\SvRec"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\StepVoice Software\SvRec\General"; ValueType: string; ValueName: "LastFilePath"; ValueData: "{userdesktop}"
;Root: HKLM; Subkey: "Software\My Company\My Program\Settings"; ValueType: string; ValueName: "Path"; ValueData: "{app}"


[Files]
Source: history.txt;          DestDir: {app}; Flags: ignoreversion
Source: license.txt;          DestDir: {app}; Flags: ignoreversion
Source: file_id.diz;          DestDir: {app}; Flags: ignoreversion
Source: home.url;             DestDir: {app}; Flags: ignoreversion
Source: order.url;            DestDir: {app}; Flags: ignoreversion

Source: ..\bin\SvRec.exe;     DestDir: {app}; Flags: ignoreversion
Source: ..\bin\lame_enc.dll;  DestDir: {app}; Flags: ignoreversion
Source: ..\doc\HtmlHelp\SvRec.chm;  DestDir: {app}; Flags: ignoreversion
Source: ..\..\bass\bass.dll;  DestDir: {app}; Flags: ignoreversion

[Icons]
;Name: {group}\Order Online; Filename: {app}\order.url; WorkingDir: {app}
Name: {group}\StepVoice Home Page;            Filename: {app}\home.url; WorkingDir: {app}
Name: {group}\StepVoice Recorder Help;        FileName: {app}\SvRec.chm; WorkingDir: {app}
Name: {group}\StepVoice Recorder;             FileName: {app}\SvRec.exe; WorkingDir: {app}
Name: {group}\Uninstall;                      FileName: {uninstallexe}
Name: {userdesktop}\StepVoice Recorder;       FileName: {app}\SvRec.exe; WorkingDir: {app}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\StepVoice Recorder; FileName: {app}\SvRec.exe; WorkingDir: {app}; Tasks: quicklaunchicon

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:
Name: quicklaunchicon; Description: Create a &Quick Launch icon; GroupDescription: Additional icons:; Flags: unchecked

[Run]
; The line below is used only for special versions.
Filename: {app}\SvRec.exe; Parameters: "/register"
FileName: {app}\SvRec.exe; Description: Launch StepVoice Recorder; Flags: postinstall nowait skipifsilent unchecked

[_ISTool]
Use7zip=false

[Code]
var
  regInfo: String; // will contain a user registration key
  
// Before INSTALL check if the program already running
function InitializeSetup(): Boolean;
begin
  if CheckForMutexes('SVREC-169A0B91-77B7-4533-9C25-59FCB08FCD614') then
  begin
    MsgBox('StepVoice Recorder is running, please close it and run again setup.',
      mbError, MB_OK);
    Result := false;
  end
  else
    Result := true;
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
