; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGraphWnd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mp3_recorder.h"
LastPage=0

ClassCount=24
Class1=CChildView
Class2=CControlButton
Class3=CCustomMenu
Class4=CEnterCodeDlg
Class5=CGraphWnd
Class6=CIcoWnd
Class7=CMainFrame
Class8=CMP3_RecorderApp
Class9=CAboutDlg
Class10=CMySheet
Class11=CMySlider
Class12=CNagScreenDlg
Class13=CPageAN
Class14=CPageCom
Class15=CPageRec
Class16=CPageSH
Class17=CPageSH2
Class18=CPageVAS
Class19=CSliderVol
Class20=CStatWnd
Class21=CSystemTray
Class22=CTimerDlg
Class23=CTimeWnd
Class24=CUrlWnd

ResourceCount=14
Resource1=IDD_ENTERCODE (English (U.S.))
Resource2=IDR_MAINFRAME (English (U.S.))
Resource3=IDD_ABOUTBOX (English (U.S.))
Resource4=IDD_SCHEDULER2 (English (U.S.))
Resource5=IDD_TIMERDLG (English (U.S.))
Resource6=IDR_TRAY_MENU
Resource7=IDD_RECORD (English (U.S.))
Resource8=IDR_GRAPHMENU (English (U.S.))
Resource9=IDR_STATMENU (English (U.S.))
Resource10=IDD_COMMON (English (U.S.))
Resource11=IDD_NAGSCREEN (English (U.S.))
Resource12=IDD_AUTONAME (English (U.S.))
Resource13=IDD_VAS (English (U.S.))
Resource14=IDT_MAINBAR (English (U.S.))

[CLS:CChildView]
Type=0
BaseClass=CWnd
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp

[CLS:CControlButton]
Type=0
BaseClass=CButton
HeaderFile=ControlButton.h
ImplementationFile=ControlButton.cpp

[CLS:CCustomMenu]
Type=0
BaseClass=CMenu
HeaderFile=CustomMenu.h
ImplementationFile=CustomMenu.cpp

[CLS:CEnterCodeDlg]
Type=0
BaseClass=CDialog
HeaderFile=EnterCodeDlg.h
ImplementationFile=EnterCodeDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CEnterCodeDlg

[CLS:CGraphWnd]
Type=0
BaseClass=CWnd
HeaderFile=GraphWnd.h
ImplementationFile=GraphWnd.cpp
LastObject=ID_GRAPH_MAXPEAKS
Filter=W
VirtualFilter=WC

[CLS:CIcoWnd]
Type=0
BaseClass=CWnd
HeaderFile=IcoWnd.h
ImplementationFile=IcoWnd.cpp
LastObject=CIcoWnd

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame

[CLS:CMP3_RecorderApp]
Type=0
BaseClass=CWinApp
HeaderFile=MP3_Recorder.h
ImplementationFile=MP3_Recorder.cpp
LastObject=CMP3_RecorderApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=MP3_Recorder.cpp
ImplementationFile=MP3_Recorder.cpp
LastObject=CAboutDlg

[CLS:CMySheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=MySheet.h
ImplementationFile=MySheet.cpp

[CLS:CMySlider]
Type=0
BaseClass=CSliderCtrl
HeaderFile=MySlider.h
ImplementationFile=MySlider.cpp

[CLS:CNagScreenDlg]
Type=0
BaseClass=CDialog
HeaderFile=NagScreenDlg.h
ImplementationFile=NagScreenDlg.cpp

[CLS:CPageAN]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageAN.h
ImplementationFile=PageAN.cpp

[CLS:CPageCom]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageCom.h
ImplementationFile=PageCom.cpp

[CLS:CPageRec]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageRec.h
ImplementationFile=PageRec.cpp

[CLS:CPageSH]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageSH.h
ImplementationFile=PageSH.cpp

[CLS:CPageSH2]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageSH2.h
ImplementationFile=PageSH2.cpp

[CLS:CPageVAS]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageVAS.h
ImplementationFile=PageVAS.cpp

[CLS:CSliderVol]
Type=0
BaseClass=CSliderCtrl
HeaderFile=SliderVol.h
ImplementationFile=SliderVol.cpp

[CLS:CStatWnd]
Type=0
BaseClass=CWnd
HeaderFile=StatWnd.h
ImplementationFile=StatWnd.cpp

[CLS:CSystemTray]
Type=0
BaseClass=CWnd
HeaderFile=SystemTray.h
ImplementationFile=SystemTray.cpp

[CLS:CTimerDlg]
Type=0
BaseClass=CDialog
HeaderFile=TimerDlg.h
ImplementationFile=TimerDlg.cpp

[CLS:CTimeWnd]
Type=0
BaseClass=CWnd
HeaderFile=TimeWnd.h
ImplementationFile=TimeWnd.cpp

[CLS:CUrlWnd]
Type=0
BaseClass=CStatic
HeaderFile=UrlWnd.h
ImplementationFile=UrlWnd.cpp
Filter=W
VirtualFilter=WC
LastObject=CUrlWnd

[DLG:IDD_ENTERCODE]
Type=1
Class=CEnterCodeDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_NAGSCREEN]
Type=1
Class=CNagScreenDlg

[DLG:IDD_AUTONAME]
Type=1
Class=CPageAN

[DLG:IDD_COMMON]
Type=1
Class=CPageCom

[DLG:IDD_RECORD]
Type=1
Class=CPageRec

[DLG:IDD_SCHEDULER]
Type=1
Class=CPageSH

[DLG:IDD_SCHEDULER2]
Type=1
Class=CPageSH2

[DLG:IDD_VAS]
Type=1
Class=CPageVAS

[DLG:IDD_TIMERDLG]
Type=1
Class=CTimerDlg

[MNU:IDR_TRAY_MENU]
Type=1
Class=?
Command1=IDM_TRAY_SHOW
Command2=IDM_TRAY_REC
Command3=IDM_TRAY_STOP
Command4=IDM_TRAY_PLAY
Command5=IDM_TRAY_ABOUT
Command6=IDM_TRAY_EXIT
CommandCount=6

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=IDM_FILE_CREATEOPEN
Command2=IDM_FILE_CLOSE
Command3=IDM_FILE_CLEAR
Command4=IDM_FILE_DELETE
Command5=IDM_FILE_FINDFILE
Command6=ID_APP_EXIT
Command7=IDM_SOUND_REC
Command8=IDM_SOUND_STOP
Command9=IDM_SOUND_PLAY
Command10=IDM_SOUND_BEGIN
Command11=IDM_SOUND_REW
Command12=IDM_SOUND_FF
Command13=IDM_SOUND_END
Command14=IDM_OPT_COM
Command15=IDM_OPT_TOP
Command16=IDM_MIX_REC
Command17=IDM_MIX_PLAY
Command18=IDM_OPT_SNDDEV
Command19=IDM_HELP_DOC
Command20=IDM_HELP_WWW
Command21=IDM_HELP_EMAIL
Command22=ID_HELP_HOWTO
Command23=ID_HELP_ORDERONLINE
Command24=ID_HELP_ENTERCODE
Command25=ID_APP_ABOUT
Command26=ID_HELP_HOWTO
CommandCount=26

[MNU:IDR_GRAPHMENU (English (U.S.))]
Type=1
Class=?
Command1=ID_GRAPH_PEAKMETER
Command2=ID_GRAPH_PEAKMETERDB
Command3=ID_GRAPH_OSCILLOSCOPE
Command4=ID_GRAPH_NONE
Command5=ID_GRAPH_MAXPEAKS
CommandCount=5

[MNU:IDR_STATMENU (English (U.S.))]
Type=1
Class=?
Command1=ID_STAT_PREF
CommandCount=1

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342373889
Control2=IDC_THANKSTO,edit,1352730628
Control3=IDC_STATIC,static,1342308353
Control4=IDC_REGNAME,static,1342308353
Control5=IDC_STATIC,static,1342308481
Control6=IDC_PLAYERICON,static,1342177539
Control7=IDC_GETNOW,static,1342308608
Control8=IDC_STATIC,button,1342177287

[DLG:IDD_COMMON (English (U.S.))]
Type=1
Class=?
ControlCount=10
Control1=IDR_OPT_BLANK,button,1342373897
Control2=IDR_OPT_NEW,button,1342177289
Control3=IDR_OPT_RELOAD,button,1342177289
Control4=IDC_AUTORUN,button,1342373891
Control5=IDC_MINIMIZED,button,1342242819
Control6=IDC_TOOLTIP_ENABLE,button,1342242819
Control7=IDC_TRAYICON,button,1342373891
Control8=IDC_TRAYMIN,button,1342373891
Control9=IDC_STATIC,button,1342177287
Control10=IDC_GEN_MINSTANCES,button,1342242819

[DLG:IDD_RECORD (English (U.S.))]
Type=1
Class=?
ControlCount=23
Control1=IDC_SLIDER1,msctls_trackbar32,1342373889
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_BITRATE_DESCR,static,1342308352
Control14=IDC_STATIC,button,1342177287
Control15=IDC_MONO,button,1342373897
Control16=IDC_STEREO,button,1342177289
Control17=IDC_STATIC,button,1342177287
Control18=IDC_SAMPLEFREQ,button,1342373897
Control19=IDC_11KHZ,button,1342177289
Control20=IDC_22KHZ,button,1342177289
Control21=IDC_44KHZ,button,1342177289
Control22=IDC_AUTOFREQ,button,1342373891
Control23=IDC_GROUP_FREQ,button,1342177287

[DLG:IDD_ENTERCODE (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDC_EDITCODE,edit,1352728644
Control2=IDC_PASTE,button,1342242816
Control3=IDC_CLEAR,button,1208025088
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDHELP,button,1208025088
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_AUTONAME (English (U.S.))]
Type=1
Class=?
ControlCount=26
Control1=IDC_STATIC,static,1342308352
Control2=IDC_AN_TDEFAULT,static,1342308608
Control3=IDE_NAMETEMPLATE,edit,1350635648
Control4=IDC_STATIC,static,1342308352
Control5=IDC_AN_EXTEXT,static,1342308352
Control6=IDC_STATIC0_1,static,1342308608
Control7=IDC_STATIC1_1,static,1342308608
Control8=IDC_STATIC2_1,static,1342308608
Control9=IDC_STATIC3_1,static,1342308608
Control10=IDC_STATIC4_1,static,1342308608
Control11=IDC_STATIC5_1,static,1342308608
Control12=IDC_STATIC6_1,static,1342308608
Control13=IDC_STATIC7_1,static,1342308608
Control14=IDC_STATIC8_1,static,1342308608
Control15=IDC_STATIC9_1,static,1342308608
Control16=IDC_STATIC0_0,static,1342308608
Control17=IDC_STATIC1_0,static,1342308608
Control18=IDC_STATIC2_0,static,1342308608
Control19=IDC_STATIC3_0,static,1342308608
Control20=IDC_STATIC4_0,static,1342308608
Control21=IDC_STATIC5_0,static,1342308608
Control22=IDC_STATIC6_0,static,1342308608
Control23=IDC_STATIC7_0,static,1342308608
Control24=IDC_STATIC8_0,static,1342308608
Control25=IDC_STATIC9_0,static,1342308608
Control26=IDC_STATIC,button,1342177287

[DLG:IDD_VAS (English (U.S.))]
Type=1
Class=?
ControlCount=19
Control1=IDC_VAS_ENABLE,button,1342242819
Control2=IDC_VAS_THRESHOLD,msctls_trackbar32,1342242817
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,button,1342177287
Control9=IDC_VAS_TIME,msctls_trackbar32,1342242817
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_VAS_SILENTPAUSE,button,1342373897
Control17=IDC_VAS_SILENTSTOP,button,1342177289
Control18=IDC_STATIC,button,1342177287
Control19=IDC_DLG_TRIALNOTE,static,1073873152

[DLG:IDD_SCHEDULER2 (English (U.S.))]
Type=1
Class=?
ControlCount=17
Control1=IDC_SH2_ENABLESHR,button,1342242819
Control2=IDC_SH2_EDITFILENAME,edit,1342179460
Control3=IDC_STATIC,button,1342177287
Control4=IDC_SH2_ENABLESTART,button,1342242819
Control5=IDC_SH2_TPSTART,SysDateTimePick32,1342242857
Control6=IDC_SH2_RUNEXTERNAL,button,1342242819
Control7=IDC_SH2_SELEXTERNAL,button,1342242880
Control8=IDC_SH2_BTN_STOPTIME,button,1342373897
Control9=IDC_SH2_BTN_RECTIME,button,1342177289
Control10=IDC_SH2_TPSTOP,SysDateTimePick32,1342242857
Control11=IDC_SH2_BTN_SHUTDOWN,button,1342242819
Control12=IDC_SH2_BTN_CLOSEALL,button,1342242819
Control13=IDC_SH2_TPRECTIME,SysDateTimePick32,1207959593
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,button,1342177287
Control16=IDC_DLG_TRIALNOTE,static,1073873152
Control17=IDC_SH2_STARTTEXT,static,1342308352

[DLG:IDD_TIMERDLG (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDCANCEL,button,1342242816
Control2=IDOK,button,1342242816
Control3=IDC_TDLG_TEXT,static,1342308352
Control4=IDC_TDLG_ICON,static,1342177283

[DLG:IDD_NAGSCREEN (English (U.S.))]
Type=1
Class=?
ControlCount=12
Control1=IDC_BUTTON1,button,1342242827
Control2=IDC_ORDER,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_ENTERCODE,button,1342242816
Control5=IDC_THANKYOU,static,1342308352
Control6=IDC_STATIC,static,1342177795
Control7=IDC_STATIC,static,1342177296
Control8=IDC_UNREGVERSION,static,1342308353
Control9=IDC_ONLINEREGISTER,static,1342308609
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342177296

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=IDA_FILE_CLEAR
Command2=IDA_OPT_EM
Command3=IDA_FILE_FINDFILE
Command4=IDA_FILE_CREATEOPEN
Command5=IDA_OPT_COM
Command6=IDA_MIX_REC
Command7=IDA_OPT_TOP
Command8=IDA_MIX_PLAY
Command9=IDA_VOL_DOWN
Command10=IDA_SOUND_END
Command11=IDA_SOUND_STOP2
Command12=IDA_HELP_DOC
Command13=IDA_SOUND_BEGIN
Command14=IDA_SOUND_REW
Command15=IDA_SOUND_REC
Command16=IDA_SOUND_FF
Command17=IDA_SOUND_PLAY
Command18=IDA_SOUND_STOP
Command19=IDA_VOL_UP
CommandCount=19

[TB:IDT_MAINBAR (English (U.S.))]
Type=1
Class=?
Command1=IDB_BTNOPEN
Command2=IDB_BTNREC
Command3=IDB_BTNSTOP
Command4=IDB_BTNPLAY
CommandCount=4

