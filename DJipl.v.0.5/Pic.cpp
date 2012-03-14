#define __GLOBAL_EXTERN__

#include "stdafx.h"
#include "Pic.h"

#include "MainFrm.h"
#include "PicFrm.h"
#include "PicDoc.h"
#include "PicView.h"
#include "Image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicApp

BEGIN_MESSAGE_MAP(CPicApp, CWinApp)
	//{{AFX_MSG_MAP(CPicApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// The one and only CPicApp object
CPicApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	CmyImage m_Image;

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CPicApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPicApp construction
CPicApp::CPicApp()
{
	m_pNewImage = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CPicApp initialization

BOOL CPicApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	AddDocTemplate(new CMultiDocTemplate(IDR_PICTYPE,
		RUNTIME_CLASS(CPicDoc),
		RUNTIME_CLASS(CPicFrame),
		RUNTIME_CLASS(CPicView)));               

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	pMainFrame->ShowWindow(SW_SHOW);
//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	pMainFrame->DragAcceptFiles();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPicApp commands

void CPicApp::OnEditPaste() 
{
	::OpenClipboard(NULL);
	if(!IsClipboardFormatAvailable(CF_DIB)) return ;

	CmyImage image;
	image.SetHandle(::CopyHandle(::GetClipboardData(CF_DIB)));
	
	CloseClipboard();

	theApp.m_pNewImage = &image;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CPicApp::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	::OpenClipboard(NULL);
	pCmdUI->Enable(IsClipboardFormatAvailable(CF_DIB));
	::CloseClipboard();
}


void CPicApp::OnFileOpen() 
{
	char szFilter[] = "Image Files(*.BMP, *.GIF, *.JPG, *.TIF)|*.BMP;*.GIF;*.JPG;*.TIF|All Files(*.*)|*.*||";

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if(IDOK == fileDlg.DoModal())
	{
		OpenDocumentFile(fileDlg.GetPathName());
	}
}

