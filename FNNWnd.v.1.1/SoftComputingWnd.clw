; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=FNNdialog
LastTemplate=splitter
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SoftComputingWnd.h"
LastPage=0

ClassCount=15
Class1=CSoftComputingWndApp
Class2=CSoftComputingWndDoc
Class3=CSoftComputingWndView
Class4=CMainFrame

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class5=CAboutDlg
Resource3=IDD_FNN
Class6=DJMessage
Class7=CMessageViewer
Resource4=IDD_ABOUTBOX (English (U.S.))
Class8=FNNdialog
Class9=DataGen
Class10=DataGenDlg
Class11=DJdummyView
Class12=CMyCamView
Class13=CMyChart
Class14=CMyScrollChart
Class15=ThinFrameSplitterWnd
Resource5=IDR_MAINFRAME (English (U.S.))

[CLS:CSoftComputingWndApp]
Type=0
HeaderFile=SoftComputingWnd.h
ImplementationFile=SoftComputingWnd.cpp
Filter=N

[CLS:CSoftComputingWndDoc]
Type=0
HeaderFile=SoftComputingWndDoc.h
ImplementationFile=SoftComputingWndDoc.cpp
Filter=N
LastObject=CSoftComputingWndDoc
BaseClass=CDocument
VirtualFilter=DC

[CLS:CSoftComputingWndView]
Type=0
HeaderFile=SoftComputingWndView.h
ImplementationFile=SoftComputingWndView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_FILE_OPEN
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=SoftComputingWnd.cpp
ImplementationFile=SoftComputingWnd.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_APP_ABOUT
CommandCount=13
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
CommandCount=13
Command4=ID_EDIT_UNDO
Command13=ID_PREV_PANE


[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_EDIT_ETC
Command12=ID_CAM_INIT
Command13=ID_CAM_CLOSE
Command14=ID_CAM_FORMAT
Command15=ID_CAM_SETTING
Command16=ID_LEARN_FNN
Command17=ID_LEARN_FMMNN
Command18=ID_LEARN_FNN2
Command19=ID_LEARN_IFNN
Command20=ID_LEARN_FEX
Command21=ID_LEARN_STOP_THREAD
Command22=ID_PARAM_SET_FNN
Command23=ID_PARAM_SET_FMMNN
Command24=ID_TEST_FNN
Command25=ID_TEST_FMMNN
Command26=TEST_FS_FAST_COMPARISON
Command27=ID_VIEW_TOOLBAR
Command28=ID_VIEW_STATUS_BAR
Command29=ID_APP_ABOUT
CommandCount=29

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:DJMessage]
Type=0
HeaderFile=DJMessage.h
ImplementationFile=DJMessage.cpp
BaseClass=CEditView
Filter=C
LastObject=DJMessage

[CLS:CMessageViewer]
Type=0
HeaderFile=MessageViewer.h
ImplementationFile=MessageViewer.cpp
BaseClass=CScrollView
Filter=C
LastObject=CMessageViewer

[DLG:IDD_FNN]
Type=1
Class=FNNdialog
ControlCount=21
Control1=IDC_RULE_COMBINATION,button,1342242819
Control2=IDC_IO_HISTOGRAM,button,1342242819
Control3=IDC_EPOCH,edit,1350631552
Control4=IDC_LEARNING_RATE,edit,1350631552
Control5=IDC_ERROR_CRITERIA,edit,1350631552
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,button,1342177287
Control12=IDC_SLP_1,edit,1350631552
Control13=IDC_SLP_2,edit,1350631552
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_SAVE_SI,button,1342242819
Control17=IDC_SAVE_RFR,button,1342242819
Control18=IDC_KILL_NODES,edit,1350631552
Control19=IDC_KILL_NODE,button,1342242819
Control20=IDC_JUST_INPUTS,edit,1350631552
Control21=IDC_JUST_INPUT,button,1342242819

[CLS:FNNdialog]
Type=0
HeaderFile=FNNdialog.h
ImplementationFile=FNNdialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=FNNdialog

[CLS:DataGen]
Type=0
HeaderFile=DataGen.h
ImplementationFile=DataGen.cpp
BaseClass=CDialog
Filter=D

[CLS:DataGenDlg]
Type=0
HeaderFile=DataGenDlg.h
ImplementationFile=DataGenDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

[CLS:DJdummyView]
Type=0
HeaderFile=DJdummyView.h
ImplementationFile=DJdummyView.cpp
BaseClass=CView
Filter=C

[CLS:CMyCamView]
Type=0
HeaderFile=MyCamView.h
ImplementationFile=MyCamView.cpp
BaseClass=CView
Filter=C
VirtualFilter=VWC
LastObject=CMyCamView

[CLS:CMyChart]
Type=0
HeaderFile=MyChart.h
ImplementationFile=MyChart.cpp
BaseClass=CView
Filter=C

[CLS:CMyScrollChart]
Type=0
HeaderFile=MyScrollChart.h
ImplementationFile=MyScrollChart.cpp
BaseClass=CView
Filter=C
VirtualFilter=VWC
LastObject=CMyScrollChart

[CLS:ThinFrameSplitterWnd]
Type=0
HeaderFile=ThinFrameSplitterWnd.h
ImplementationFile=ThinFrameSplitterWnd.cpp
BaseClass=splitter
Filter=T
LastObject=ThinFrameSplitterWnd

