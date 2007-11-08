; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCustomMenu
LastTemplate=CMenu
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mp3_recorder.h"
LastPage=0

ClassCount=14
Class1=CChildView
Class2=CControlButton
Class3=CGraphWnd
Class4=CIcoWnd
Class5=CMainFrame
Class6=CMP3_RecorderApp
Class7=CAboutDlg
Class8=CMySheet
Class9=CMySlider
Class10=CPageCom
Class11=CPageRec
Class12=CStatWnd
Class13=CTimeWnd
Class14=CCustomMenu

ResourceCount=5
Resource1=IDD_ABOUTBOX (English (U.S.))
Resource2=IDD_COMMON (English (U.S.))
Resource3=IDR_MIXER_SELECT_MENU (English (U.S.))
Resource4=IDR_MAINFRAME (English (U.S.))
Resource5=IDD_RECORD (English (U.S.))

[CLS:CChildView]
Type=0
BaseClass=CWnd 
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=W
VirtualFilter=WC
LastObject=CChildView

[CLS:CControlButton]
Type=0
BaseClass=CButton
HeaderFile=ControlButton.h
ImplementationFile=ControlButton.cpp
Filter=W
VirtualFilter=BWC
LastObject=CControlButton

[CLS:CGraphWnd]
Type=0
BaseClass=CWnd
HeaderFile=GraphWnd.h
ImplementationFile=GraphWnd.cpp
Filter=W
VirtualFilter=WC
LastObject=CGraphWnd

[CLS:CIcoWnd]
Type=0
BaseClass=CWnd
HeaderFile=IcoWnd.h
ImplementationFile=IcoWnd.cpp
Filter=W
VirtualFilter=WC
LastObject=CIcoWnd

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=CMainFrame

[CLS:CMP3_RecorderApp]
Type=0
BaseClass=CWinApp
HeaderFile=MP3_Recorder.h
ImplementationFile=MP3_Recorder.cpp
LastObject=CMP3_RecorderApp
Filter=N
VirtualFilter=AC

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
LastObject=CMySheet
Filter=W
VirtualFilter=hWC

[CLS:CMySlider]
Type=0
BaseClass=CSliderCtrl
HeaderFile=MySlider.h
ImplementationFile=MySlider.cpp
Filter=W
VirtualFilter=KWC
LastObject=CMySlider

[CLS:CPageCom]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageCom.h
ImplementationFile=PageCom.cpp
LastObject=CPageCom
Filter=D
VirtualFilter=idWC

[CLS:CPageRec]
Type=0
BaseClass=CPropertyPage
HeaderFile=PageRec.h
ImplementationFile=PageRec.cpp
LastObject=IDC_AUTOFREQ

[CLS:CStatWnd]
Type=0
BaseClass=CWnd
HeaderFile=StatWnd.h
ImplementationFile=StatWnd.cpp
Filter=W
VirtualFilter=WC
LastObject=CStatWnd

[CLS:CTimeWnd]
Type=0
BaseClass=CWnd
HeaderFile=TimeWnd.h
ImplementationFile=TimeWnd.cpp
Filter=W
VirtualFilter=WC
LastObject=CTimeWnd

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_COMMON]
Type=1
Class=CPageCom

[DLG:IDD_RECORD]
Type=1
Class=CPageRec

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=IDM_FILE_NEW
Command2=IDM_FILE_OPEN
Command3=IDM_FILE_CLOSE
Command4=IDM_FILE_FINDTARGET
Command5=ID_APP_EXIT
Command6=IDM_SOUND_REC
Command7=IDM_SOUND_PLAY
Command8=IDM_SOUND_BEGIN
Command9=IDM_SOUND_REW
Command10=IDM_SOUND_FF
Command11=IDM_SOUND_END
Command12=IDM_OPT_COM
Command13=IDM_OPT_TOP
Command14=IDM_OPT_EM
Command15=IDM_MIX_REC
Command16=IDM_MIX_PLAY
Command17=IDM_HELP_DOC
Command18=IDM_HELP_GETFULL
Command19=IDM_HELP_WWW
Command20=IDM_HELP_EMAIL
Command21=ID_APP_ABOUT
CommandCount=21

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=IDA_OPT_TOP
Command2=IDA_OPT_EM
Command3=IDA_FILE_NEW
Command4=IDA_FILE_OPEN
Command5=IDA_OPT_COM
Command6=IDA_MIX_REC
Command7=IDA_MIX_PLAY
Command8=IDA_SOUND_END
Command9=IDA_SOUND_BEGIN
Command10=IDA_SOUND_REW
Command11=IDA_SOUND_REC
Command12=IDA_SOUND_FF
Command13=IDA_SOUND_PLAY
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDC_STATIC,static,1342177539
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342177288
Control7=IDC_STATIC,static,1342177288
Control8=IDC_STATIC,static,1342308352

[DLG:IDD_COMMON (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,button,1342177287
Control2=IDR_OPT_BLANK,button,1342242819
Control3=IDC_STATIC,button,1342177287
Control4=IDC_AUTORUN,button,1342373891
Control5=IDC_MINIMIZED,button,1342242819

[DLG:IDD_RECORD (English (U.S.))]
Type=1
Class=?
ControlCount=13
Control1=IDC_STATIC,button,1342177287
Control2=IDC_SLIDER1,msctls_trackbar32,1342373889
Control3=IDC_STATIC,static,1342308352
Control4=IDC_BITRATE_DESCR,static,1342308352
Control5=IDC_STATIC,button,1342177287
Control6=IDC_MONO,button,1342373897
Control7=IDC_STEREO,button,1342177289
Control8=IDC_GROUP_FREQ,button,1342177287
Control9=IDC_SAMPLEFREQ,button,1342373897
Control10=IDC_11KHZ,button,1342177289
Control11=IDC_22KHZ,button,1342177289
Control12=IDC_44KHZ,button,1342177289
Control13=IDC_AUTOFREQ,button,1342373891

[MNU:IDR_MIXER_SELECT_MENU (English (U.S.))]
Type=1
Class=?
Command1=ID_TEST1_MICROPHONE
Command2=ID_TEST1_LINEIN
Command3=ID_TEST1_STEREOMIX
Command4=ID_TEST1_VOLUME
CommandCount=4

[CLS:CCustomMenu]
Type=0
HeaderFile=CustomMenu.h
ImplementationFile=CustomMenu.cpp
BaseClass=CMenu
Filter=W
LastObject=CCustomMenu

