; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CFormView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SoftComputingWnd.h"
LastPage=0

ClassCount=16
Class1=CSoftComputingWndApp
Class2=CSoftComputingWndDoc
Class3=CSoftComputingWndView
Class4=CMainFrame

ResourceCount=20
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class5=CAboutDlg
Resource3=IDD_CMD_ALU
Class6=DJMessage
Resource4=IDD_ABOUTBOX (English (U.S.))
Class7=CCmdColor
Resource5=IDD_CMD_FNN
Class8=CCmdFilter
Resource6=IDD_CMD_TEMPLATE_MATCHING
Class9=CCmdFourier
Resource7=IDD_CMD_GF
Resource8=IDD_CMD_COORD
Resource9=IDD_VERIFY
Resource10=IDD_CMD_SF
Resource11=IDD_CMD_FMMNN
Resource12=IDD_CMD_BLOB
Class10=CCmdBlob
Class11=CCmdTM
Resource13=IDD_CMD_THRESHOLD
Resource14=IDD_CMD_FILTER
Class12=CCmdVer
Resource15=IDD_CMD_COLOR
Resource16=IDD_CMD_EIGEN
Resource17=IDD_CMD_PROJ_EXT
Resource18=IDD_CMD_NN
Resource19=IDD_CMD_FTX
Class13=CCmdFMMNN
Class14=CCmdNN
Class15=CCmdFNN
Class16=CCmdEigen
Resource20=IDR_MAINFRAME (English (U.S.))

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
LastObject=CSoftComputingWndView
BaseClass=CView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_WNN_TEST
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
Command7=ID_WNN_LEARNING
Command8=ID_WNN_TEST
CommandCount=8

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

[DLG:IDD_CMD_COLOR]
Type=1
Class=CCmdColor
ControlCount=5
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SOURCE,combobox,1344339970
Control3=IDC_STATIC,static,1342308352
Control4=IDC_RESULT,combobox,1344339970
Control5=IDC_ACT_CONVERT,button,1342242816

[CLS:CCmdColor]
Type=0
HeaderFile=CmdColor.h
ImplementationFile=CmdColor.cpp
BaseClass=CFormView
Filter=D
LastObject=CCmdColor
VirtualFilter=VWC

[DLG:IDD_CMD_FILTER]
Type=1
Class=CCmdFilter
ControlCount=17
Control1=IDC_FILTERING_METHOD,combobox,1344339970
Control2=IDC_STATIC,static,1342308352
Control3=IDC_ACT_FILTERING,button,1342242816
Control4=IDC_LOW_TH,msctls_trackbar32,1342242840
Control5=IDC_HIGH_TH,msctls_trackbar32,1342242840
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_LOW_TH_VAL,edit,1342244864
Control9=IDC_HIGH_TH_VAL,edit,1342244864
Control10=IDC_GABOR_ANGLE,msctls_trackbar32,1342242840
Control11=IDC_STATIC,static,1342308352
Control12=IDC_GABOR_ANGLE_VAL,edit,1342244864
Control13=IDC_GABOR_WAVE,msctls_trackbar32,1342242840
Control14=IDC_STATIC,static,1342308352
Control15=IDC_GABOR_WAVE_VAL,edit,1342244864
Control16=IDC_STATIC,button,1342177287
Control17=IDC_STATIC,button,1342177287

[CLS:CCmdFilter]
Type=0
HeaderFile=CmdFilter.h
ImplementationFile=CmdFilter.cpp
BaseClass=CFormView
Filter=D
VirtualFilter=VWC
LastObject=CCmdFilter

[DLG:IDD_CMD_FTX]
Type=1
Class=CCmdFourier
ControlCount=3
Control1=IDC_BUTTON1,button,1342242816
Control2=IDC_FOURIER,combobox,1344339970
Control3=IDC_STATIC,static,1342308352

[CLS:CCmdFourier]
Type=0
HeaderFile=CmdFourier.h
ImplementationFile=CmdFourier.cpp
BaseClass=CFormView
Filter=D
VirtualFilter=VWC
LastObject=CCmdFourier

[DLG:IDD_CMD_BLOB]
Type=1
Class=CCmdBlob
ControlCount=6
Control1=IDC_BLOB_ANAL,button,1342242816
Control2=IDC_BLOB_METHOD,combobox,1344339970
Control3=IDC_STATIC,static,1342308352
Control4=IDC_MORPH,combobox,1344339970
Control5=IDC_STATIC,static,1342308352
Control6=IDC_MORPH_NUM,edit,1350631552

[DLG:IDD_CMD_GF]
Type=1
Class=?
ControlCount=5
Control1=IDC_GF_EXT,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_GF_LOCAL,combobox,1344339970
Control5=IDC_GF_GLOBAL,combobox,1344339970

[DLG:IDD_CMD_SF]
Type=1
Class=?
ControlCount=3
Control1=IDC_SF_EXT,button,1342242816
Control2=IDC_SF_FEAT,combobox,1344340226
Control3=IDC_STATIC,static,1342308352

[CLS:CCmdBlob]
Type=0
HeaderFile=CmdBlob.h
ImplementationFile=CmdBlob.cpp
BaseClass=CFormView
Filter=D
LastObject=CCmdBlob
VirtualFilter=VWC

[DLG:IDD_CMD_COORD]
Type=1
Class=?
ControlCount=5
Control1=IDC_COORD_TX,button,1342242816
Control2=IDC_SRC_COORD,combobox,1344339970
Control3=IDC_TAR_COORD,combobox,1344339970
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_CMD_PROJ_EXT]
Type=1
Class=?
ControlCount=3
Control1=IDC_PROJ_EX,button,1342242816
Control2=IDC_PROJ_DIR,combobox,1344339970
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_CMD_TEMPLATE_MATCHING]
Type=1
Class=CCmdTM
ControlCount=8
Control1=IDC_TEMP_MATCH,button,1342242816
Control2=IDC_TM_METHOD,combobox,1344339970
Control3=IDC_STATIC,static,1342308352
Control4=IDC_TM_METHOD2,combobox,1344339970
Control5=IDC_STATIC,static,1342308352
Control6=IDC_MATCH_DEGREE,msctls_trackbar32,1342242840
Control7=IDC_STATIC,static,1342308352
Control8=IDC_MATCH_DEGREE_VAL,edit,1342244992

[CLS:CCmdTM]
Type=0
HeaderFile=CmdTM.h
ImplementationFile=CmdTM.cpp
BaseClass=CFormView
Filter=D
LastObject=CCmdTM
VirtualFilter=VWC

[DLG:IDD_CMD_THRESHOLD]
Type=1
Class=?
ControlCount=9
Control1=IDC_ACT_TH,button,1342242816
Control2=IDC_MIN_TH,msctls_trackbar32,1342242840
Control3=IDC_MAX_TH,msctls_trackbar32,1342242840
Control4=IDC_TH_METHOD,combobox,1344339970
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_MAX_THR,edit,1342244864
Control9=IDC_MIN_THR,edit,1342244864

[DLG:IDD_VERIFY]
Type=1
Class=CCmdVer
ControlCount=14
Control1=IDC_ACT_VER,button,1342242816
Control2=IDC_FACE_VER_VAL,edit,1342244992
Control3=IDC_FACE_VER,msctls_trackbar32,1342242840
Control4=IDC_STATIC,static,1342308352
Control5=IDC_LEFT_EYEB,edit,1350631552
Control6=IDC_RIGHT_EYEB,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_NOSE,edit,1350631552
Control9=IDC_MOUTH,edit,1350631552
Control10=IDC_FOREHEAD,edit,1350631552
Control11=IDC_INTERDIST,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_INTERNM,edit,1350631552
Control14=IDC_JAW,edit,1350631552

[CLS:CCmdVer]
Type=0
HeaderFile=CmdVer.h
ImplementationFile=CmdVer.cpp
BaseClass=CFormView
Filter=D
VirtualFilter=VWC
LastObject=IDC_ACT_VER

[DLG:IDD_CMD_ALU]
Type=1
Class=?
ControlCount=8
Control1=IDC_ACT_ALU,button,1342242816
Control2=IDC_OPERATOR,combobox,1344340034
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CHECK1,button,1342242819
Control5=IDC_CHECK2,button,1342242819
Control6=IDC_CHECK3,button,1342242819
Control7=IDC_CONST,edit,1350631552
Control8=IDC_STATIC,static,1342308352

[DLG:IDD_CMD_NN]
Type=1
Class=CCmdNN
ControlCount=12
Control1=IDC_REC_NN,button,1342242816
Control2=IDC_STATIC,button,1342177287
Control3=IDC_NN_METHOD,combobox,1344340226
Control4=IDC_STATIC,static,1342308352
Control5=IDC_NN_EP,edit,1350631552
Control6=IDC_NN_ERR,edit,1350631552
Control7=IDC_NN_LR,edit,1350631552
Control8=IDC_NN_MO,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352

[DLG:IDD_CMD_FMMNN]
Type=1
Class=CCmdFMMNN
ControlCount=8
Control1=IDC_REC_FMMNN,button,1342242816
Control2=IDC_STATIC,button,1342177287
Control3=IDC_FMMNN_METHOD,combobox,1344340226
Control4=IDC_STATIC,static,1342308352
Control5=IDC_FMMNN_R,edit,1350631552
Control6=IDC_FMMNN_THETA,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[DLG:IDD_CMD_FNN]
Type=1
Class=CCmdFNN
ControlCount=14
Control1=IDC_REC_FNN,button,1342242816
Control2=IDC_STATIC,button,1342177287
Control3=IDC_FNN_METHOD,combobox,1344340226
Control4=IDC_STATIC,static,1342308352
Control5=IDC_FNN_EP,edit,1350631552
Control6=IDC_FNN_ERR,edit,1350631552
Control7=IDC_FNN_LR,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_FNN_SP1,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_FNN_SP2,edit,1350631552
Control14=IDC_STATIC,static,1342308352

[DLG:IDD_CMD_EIGEN]
Type=1
Class=CCmdEigen
ControlCount=8
Control1=IDC_REC_EIGEN,button,1342242816
Control2=IDC_STATIC,button,1342177287
Control3=IDC_EIGEN_METHOD,combobox,1344340226
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EIGEN_EP,edit,1350631552
Control6=IDC_EIGEN_ERR,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:CCmdFMMNN]
Type=0
HeaderFile=CmdFMMNN.h
ImplementationFile=CmdFMMNN.cpp
BaseClass=CFormView
Filter=D
LastObject=CCmdFMMNN
VirtualFilter=VWC

[CLS:CCmdNN]
Type=0
HeaderFile=CmdNN.h
ImplementationFile=CmdNN.cpp
BaseClass=CFormView
Filter=D
LastObject=CCmdNN
VirtualFilter=VWC

[CLS:CCmdFNN]
Type=0
HeaderFile=CmdFNN.h
ImplementationFile=CmdFNN.cpp
BaseClass=CFormView
Filter=D
VirtualFilter=VWC
LastObject=IDC_REC_FNN

[CLS:CCmdEigen]
Type=0
HeaderFile=CmdEigen.h
ImplementationFile=CmdEigen.cpp
BaseClass=CFormView
Filter=D
LastObject=IDC_REC_EIGEN
VirtualFilter=VWC

