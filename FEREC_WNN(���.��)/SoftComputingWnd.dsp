# Microsoft Developer Studio Project File - Name="SoftComputingWnd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SoftComputingWnd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SoftComputingWnd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SoftComputingWnd.mak" CFG="SoftComputingWnd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SoftComputingWnd - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SoftComputingWnd - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SoftComputingWnd - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "SoftComputingWnd - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "HAVE_IPL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ipl.lib highgui.lib cv.lib RSDLL.lib rpl.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SoftComputingWnd - Win32 Release"
# Name "SoftComputingWnd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CmdAlu.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdBlob.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdColor.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdCoord.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdEigen.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdFMMNN.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdFNN.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdFourier.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdGF.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdNN.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdProjExt.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdSF.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdTh.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdTM.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdVer.cpp
# End Source File
# Begin Source File

SOURCE=.\FMMNN.cpp
# End Source File
# Begin Source File

SOURCE=.\FRTK.cpp
# End Source File
# Begin Source File

SOURCE=.\GaborFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageBaseView.cpp
# End Source File
# Begin Source File

SOURCE=.\LogView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\myFNN.cpp
# End Source File
# Begin Source File

SOURCE=.\NNfunction.cpp
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWnd.rc
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWndDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWndView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SVV.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CmdAlu.h
# End Source File
# Begin Source File

SOURCE=.\CmdBlob.h
# End Source File
# Begin Source File

SOURCE=.\CmdColor.h
# End Source File
# Begin Source File

SOURCE=.\CmdCoord.h
# End Source File
# Begin Source File

SOURCE=.\CmdEigen.h
# End Source File
# Begin Source File

SOURCE=.\CmdFilter.h
# End Source File
# Begin Source File

SOURCE=.\CmdFMMNN.h
# End Source File
# Begin Source File

SOURCE=.\CmdFNN.h
# End Source File
# Begin Source File

SOURCE=.\CmdFourier.h
# End Source File
# Begin Source File

SOURCE=.\CmdGF.h
# End Source File
# Begin Source File

SOURCE=.\CmdNN.h
# End Source File
# Begin Source File

SOURCE=.\CmdProjExt.h
# End Source File
# Begin Source File

SOURCE=.\CmdSF.h
# End Source File
# Begin Source File

SOURCE=.\CmdTh.h
# End Source File
# Begin Source File

SOURCE=.\CmdTM.h
# End Source File
# Begin Source File

SOURCE=.\CmdVer.h
# End Source File
# Begin Source File

SOURCE=.\FMMNN.h
# End Source File
# Begin Source File

SOURCE=.\FRTK.h
# End Source File
# Begin Source File

SOURCE=.\GaborFilter.h
# End Source File
# Begin Source File

SOURCE=.\ImageBaseView.h
# End Source File
# Begin Source File

SOURCE=.\LogView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\myFNN.h
# End Source File
# Begin Source File

SOURCE=.\NNfunction.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWnd.h
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWndDoc.h
# End Source File
# Begin Source File

SOURCE=.\SoftComputingWndView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SVV.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SoftComputingWnd.ico
# End Source File
# Begin Source File

SOURCE=.\res\SoftComputingWnd.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SoftComputingWndDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
