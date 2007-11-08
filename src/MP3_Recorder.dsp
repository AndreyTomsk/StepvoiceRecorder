# Microsoft Developer Studio Project File - Name="MP3_Recorder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MP3_Recorder - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MP3_Recorder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MP3_Recorder.mak" CFG="MP3_Recorder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MP3_Recorder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MP3_Recorder - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MP3_Recorder - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib htmlhelp.lib /nologo /version:1.5 /subsystem:windows /map /machine:I386 /out:"Release/SvRec.exe" /mapinfo:lines /OPT:REF
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "MP3_Recorder - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/StepVoice Recorder.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "MP3_Recorder - Win32 Release"
# Name "MP3_Recorder - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Interface\Buttons\bitmapbtn.cpp
# ADD CPP /I "..\.."
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlMux.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder_MP3.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder_MP3.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder_WAV.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterCodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\IcoWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MappedMP3.cpp
# End Source File
# Begin Source File

SOURCE=.\Mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\MixerFader.cpp
# End Source File
# Begin Source File

SOURCE=.\MixerPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\MixerRec.cpp
# End Source File
# Begin Source File

SOURCE=.\MP3_Recorder.cpp
# End Source File
# Begin Source File

SOURCE=.\MP3_Recorder.rc
# End Source File
# Begin Source File

SOURCE=.\MySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MySlider.cpp
# End Source File
# Begin Source File

SOURCE=.\NagScreenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PageAN.cpp
# End Source File
# Begin Source File

SOURCE=.\PageCom.cpp
# End Source File
# Begin Source File

SOURCE=.\PageRec.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSH2.cpp
# End Source File
# Begin Source File

SOURCE=.\PageVAS.cpp
# End Source File
# Begin Source File

SOURCE=.\PROTECT.CPP
# End Source File
# Begin Source File

SOURCE=.\Scheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\Scheduler2.cpp
# End Source File
# Begin Source File

SOURCE=.\SliderVol.cpp
# End Source File
# Begin Source File

SOURCE=.\SndFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SndFile_MP3.cpp
# End Source File
# Begin Source File

SOURCE=.\StatWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\system.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemTray.cpp
# End Source File
# Begin Source File

SOURCE=.\TimerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleText.cpp
# End Source File
# Begin Source File

SOURCE=.\UrlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\VAS.cpp
# End Source File
# Begin Source File

SOURCE=.\Wave.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveIn.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveOut.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AfStack.h
# End Source File
# Begin Source File

SOURCE=.\Interface\Buttons\bitmapbtn.h
# End Source File
# Begin Source File

SOURCE=.\BladeMP3EncDLL.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\ControlButton.h
# End Source File
# Begin Source File

SOURCE=.\ControlMux.h
# End Source File
# Begin Source File

SOURCE=.\CustomMenu.h
# End Source File
# Begin Source File

SOURCE=.\Decoder.h
# End Source File
# Begin Source File

SOURCE=.\Decoder_MP3.h
# End Source File
# Begin Source File

SOURCE=.\Encoder.h
# End Source File
# Begin Source File

SOURCE=.\Encoder_MP3.h
# End Source File
# Begin Source File

SOURCE=.\Encoder_WAV.h
# End Source File
# Begin Source File

SOURCE=.\EnterCodeDlg.h
# End Source File
# Begin Source File

SOURCE=.\GraphWnd.h
# End Source File
# Begin Source File

SOURCE=.\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=.\IcoWnd.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MappedMP3.h
# End Source File
# Begin Source File

SOURCE=.\Mixer.h
# End Source File
# Begin Source File

SOURCE=.\MixerFader.h
# End Source File
# Begin Source File

SOURCE=.\MixerPlay.h
# End Source File
# Begin Source File

SOURCE=.\MixerRec.h
# End Source File
# Begin Source File

SOURCE=.\MP3_Recorder.h
# End Source File
# Begin Source File

SOURCE=.\mpglibdll.h
# End Source File
# Begin Source File

SOURCE=.\MySheet.h
# End Source File
# Begin Source File

SOURCE=.\MySlider.h
# End Source File
# Begin Source File

SOURCE=.\NagScreenDlg.h
# End Source File
# Begin Source File

SOURCE=.\PageAN.h
# End Source File
# Begin Source File

SOURCE=.\PageCom.h
# End Source File
# Begin Source File

SOURCE=.\PageRec.h
# End Source File
# Begin Source File

SOURCE=.\PageSH2.h
# End Source File
# Begin Source File

SOURCE=.\PageVAS.h
# End Source File
# Begin Source File

SOURCE=.\PROTECT.H
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Scheduler.h
# End Source File
# Begin Source File

SOURCE=.\Scheduler2.h
# End Source File
# Begin Source File

SOURCE=.\SliderVol.h
# End Source File
# Begin Source File

SOURCE=.\SndFile.h
# End Source File
# Begin Source File

SOURCE=.\SndFile_MP3.h
# End Source File
# Begin Source File

SOURCE=.\StatWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\struct.h
# End Source File
# Begin Source File

SOURCE=.\system.h
# End Source File
# Begin Source File

SOURCE=.\SystemTray.h
# End Source File
# Begin Source File

SOURCE=.\TimerDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimeWnd.h
# End Source File
# Begin Source File

SOURCE=.\TitleText.h
# End Source File
# Begin Source File

SOURCE=.\UrlWnd.h
# End Source File
# Begin Source File

SOURCE=.\VAS.h
# End Source File
# Begin Source File

SOURCE=.\Wave.h
# End Source File
# Begin Source File

SOURCE=.\WaveIn.h
# End Source File
# Begin Source File

SOURCE=.\WaveOut.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\arrow2.cur
# End Source File
# Begin Source File

SOURCE=.\RES\big_digits.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmpnull.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmppause.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmpplay.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmpstop.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\btn_mon.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\btn_sched.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\btn_vas.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\RES\digital_window.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\din.ico
# End Source File
# Begin Source File

SOURCE=.\RES\din2.ico
# End Source File
# Begin Source File

SOURCE=.\RES\din_m.ico
# End Source File
# Begin Source File

SOURCE=.\RES\downarro.ico
# End Source File
# Begin Source File

SOURCE=.\RES\graph_wn.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\graph_wnd.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\gw_numbe.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\gwbg_dot.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\gwbg_lgp.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\gwbg_lnp.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\gwbg_non.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\hand.cur
# End Source File
# Begin Source File

SOURCE=.\RES\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\icons_proc.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\RES\lin.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mainbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\mic.ico
# End Source File
# Begin Source File

SOURCE=.\Ico\MIKE.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\mixerbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\mixline0.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mixsel.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mono_stereo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MP3_Rec_test.ico
# End Source File
# Begin Source File

SOURCE=.\res\MP3_Recorder.ico
# End Source File
# Begin Source File

SOURCE=.\res\MP3_Recorder.rc2
# End Source File
# Begin Source File

SOURCE=.\RES\new.ico
# End Source File
# Begin Source File

SOURCE=.\RES\open2.ico
# End Source File
# Begin Source File

SOURCE=.\RES\otr.ico
# End Source File
# Begin Source File

SOURCE=.\RES\pause.ico
# End Source File
# Begin Source File

SOURCE=.\RES\peak_meter.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\peakmete.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\pm_lin.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\pmlg_24.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\pmln_24.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\sel.ico
# End Source File
# Begin Source File

SOURCE=.\RES\small_digits.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\stat_bitmaps.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\stat_bitmaps2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\stat_wnd.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\stat_wnd2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\stm.ico
# End Source File
# Begin Source File

SOURCE=.\RES\stop.ico
# End Source File
# Begin Source File

SOURCE=.\RES\time_wnd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\tray_pau.ico
# End Source File
# Begin Source File

SOURCE=.\RES\tray_pla.ico
# End Source File
# Begin Source File

SOURCE=.\RES\tray_rec.ico
# End Source File
# Begin Source File

SOURCE=.\RES\tray_sto.ico
# End Source File
# Begin Source File

SOURCE=.\Ico\VOLUME01.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
