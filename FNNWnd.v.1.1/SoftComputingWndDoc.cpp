// SoftComputingWndDoc.cpp : implementation of the CSoftComputingWndDoc class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"

#include "SoftComputingWndDoc.h"

#include "MainFrm.h"

#include "ImageBaseView.h"
#include "LogView.h"
#include "cv.h"
#include "highgui.h"

#include "myFNN.h"

#include "FNNdialog.h"

#include "IplFace.h"

#include "MyScrollChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc

IMPLEMENT_DYNCREATE(CSoftComputingWndDoc, CDocument)

BEGIN_MESSAGE_MAP(CSoftComputingWndDoc, CDocument)
	//{{AFX_MSG_MAP(CSoftComputingWndDoc)
	ON_COMMAND(ID_SHOW_IMG, OnShowImg)
	ON_COMMAND(ID_TEST_FNN, OnTestFnn)
	ON_COMMAND(ID_LEARN_FMMNN, OnLearnFmmnn)
	ON_COMMAND(ID_PARAM_SET_FNN, OnParamSetFnn)
	ON_COMMAND(ID_LEARN_FNN, OnLearnFnn)
	ON_COMMAND(ID_CAM_CLOSE, OnCamClose)
	ON_COMMAND(ID_CAM_FORMAT, OnCamFormat)
	ON_COMMAND(ID_CAM_INIT, OnCamInit)
	ON_COMMAND(ID_TEST_FMMNN, OnTestFmmnn)
	ON_COMMAND(ID_CAM_SETTING, OnCamSetting)
	ON_COMMAND(ID_EDIT_ETC, OnEditEtc)
	ON_COMMAND(ID_LEARN_FNN2, OnLearnFnn2)
	ON_COMMAND(ID_LEARN_IFNN, OnLearnIfnn)
	ON_COMMAND(TEST_FS_FAST_COMPARISON, OnFsFastComparison)
	ON_COMMAND(ID_LEARN_FEX, OnLearnFex)
	ON_COMMAND(ID_LEARN_STOP_THREAD, OnLearnStopThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc construction/destruction

CSoftComputingWndDoc::CSoftComputingWndDoc()
{
	fnnLoaded = FALSE;
	fmmnnLoaded = FALSE;
	imageLoaded = FALSE;
	datLoaded = FALSE;
	bxdLoaded = FALSE;
}

CSoftComputingWndDoc::~CSoftComputingWndDoc()
{
}

BOOL CSoftComputingWndDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc serialization

void CSoftComputingWndDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc diagnostics

#ifdef _DEBUG
void CSoftComputingWndDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSoftComputingWndDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc commands

void CSoftComputingWndDoc::OnShowImg() 
{
	if (!imageLoaded)
		return;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CImageBaseView* imageview = mainframe->GetImageBaseView();
	imageview->SetImagePlane(4, 2);
	CLogView* logview = mainframe->GetLogView();
	logview->AddLine("Test Show Image");

	imageview->AddImage(2, 0, m_image);
	imageview->AddImage(3, 1, m_image);

	imageview->ShowImage(TRUE);
}

void CSoftComputingWndDoc::OnTestFnn() 
{
	if (!fnnLoaded)
		return;

	// prepare initial config.
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	m_fnn.imgview = mainframe->GetImageBaseView();
	m_fnn.imgview->SetImagePlane(1, 1);
	m_fnn.logview = mainframe->GetLogView();
	m_fnn.chartview[0] = mainframe->GetChart(0);
	m_fnn.chartview[1] = mainframe->GetChart(1);
	m_fnn.scrollchartview = mainframe->GetScrollChart();

	CFileDialog *pDlg = new CFileDialog( TRUE, NULL, NULL, OFN_READONLY, 
 		         "입력패턴파일 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

   	//pDlg->m_ofn.lpstrInitialDir = "h:\\user\\djkim\\research\\archive\\program\\ongoing\\fnnwnd\\";	//폴더 지정

	// parsing
	char seps[] = " ";
	char *token;
	int no_nodes = 0;
	int* nodes = new int[m_fnn.layer[1]];
	int n;

	token = strtok((char*)(LPCTSTR)FNNkillnodes, seps);

	while (token != NULL)
	{
		nodes[no_nodes++] = atoi(token);
		token = strtok( NULL, seps);
	}


	if( pDlg->DoModal() )
	{
	   filename = pDlg->GetFileName();
   
	   if( filename != "" )  
	   {
		   m_fnn.loadDataN1(filename);

	////////////////////////////////////////////////////
	// parsing
		   /*
	char seps2[] = " ";
	char *token2;
	int no_nodes2 = 0;
	int* nodes2 = new int[m_fnn.layer[4]];

	token2 = strtok((char*)(LPCTSTR)FNNjustinputs, seps2);

	while (token2 != NULL)
	{
		nodes2[no_nodes2++] = atoi(token2);
		token2 = strtok( NULL, seps2);
	}
	*/

	// Inc. Learning -> Model Selection
	//m_fnn.DataTransform(nodes2, no_nodes2);

	/////////////////////////////////////////////////////////


			m_fnn.initFNN();
			m_fnn.ioHistogram();
			m_fnn.ShowFNND();

			/*
			n = filename.Find(".");
			filename.Delete(n, 4);
			filename.Insert(n, "1.ioh");
			m_fnn.SaveIOHistogram1(filename);
			*/

			/*
			n = filename.Find(".");
			filename.Delete(n, 4);
			filename.Insert(n, "0.ioh");
			m_fnn.SaveIOHistogram(filename);

			n = filename.Find(".");
			filename.Delete(n, 4);
			filename.Insert(n, "1.ioh");
			m_fnn.SaveIOHistogram1(filename);

			n = filename.Find(".");
			filename.Delete(n, 4);
			filename.Insert(n, "2.ioh");
			m_fnn.SaveIOHistogram2(filename);

			n = filename.Find(".");
			filename.Delete(n, 4);
			filename.Insert(n, "3.ioh");
			m_fnn.SaveIOHistogram3(filename);
			*/

			// delete input nodes
			m_fnn.deleteInputNodes(nodes, no_nodes);
			//

			//m_fnn.ShowMF(0); 	m_fnn.ShowMF(1);

			//m_fnn.recTrain();
			float clr;
			m_fnn.recTest(&clr);

//	delete [] nodes2;

	   }
	}

	delete [] nodes;
}

BOOL CSoftComputingWndDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	CString filetype = lpszPathName;
	filename = filetype;

	if (filetype.Find(".dat") > -1)
	{
		datLoaded = TRUE;
		m_fnn.loadDataN1(lpszPathName);
		int n = filetype.Find(".dat");
		filetype.Delete(n, 4);
		filetype.Insert(n, ".fn");
		m_fnn.loadFN(filetype);

		//20040102
		CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
		CMyScrollChart* scrollchart = (CMyScrollChart*)(mainframe->GetScrollChart());
		scrollchart->SetScrollChart();
	}
	else if (filetype.Find(".fnn") > -1)
	{
		fnnLoaded = TRUE;
		// load fnn file

		/*
		if (m_fnn)
		{
			delete m_fnn;
			m_fnn = NULL;
		}
		else
		*/
		
		//m_fnn = new myFNN;

		m_fnn.loadFNN(filetype);
	}
	else if ((filetype.Find(".jpg") > -1) 
		|| (filetype.Find(".gif") > -1)
		|| (filetype.Find(".bmp") > -1)
		|| (filetype.Find(".tif") > -1))
	{
		fmmnnLoaded = TRUE;
		m_image = cvvLoadImage((char*)lpszPathName);
	}
	else if (filetype.Find(".fmm") > -1)
	{
		fmmnnLoaded = TRUE;
	}
	else if (filetype.Find(".bxd") > -1)
	{
		bxdLoaded = TRUE;
	}
	
	return TRUE;
}

BOOL CSoftComputingWndDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	CString filetype = lpszPathName;

	if (filetype.Find(".fnn") > -1)
	{
		m_fnn.saveFNN(filetype);
	}
	else if ((filetype.Find(".jpg") > -1) 
		|| (filetype.Find(".gif") > -1)
		|| (filetype.Find(".bmp") > -1)
		|| (filetype.Find(".tif") > -1))
	{
		cvvSaveImage(lpszPathName, m_image);
	}	
	else if (filetype.Find(".bxd") > -1)
	{
		m_fmmnn.SaveBox2(filetype);
	}

	return TRUE;
}

void CSoftComputingWndDoc::OnCloseDocument() 
{
	if (fnnLoaded || datLoaded)
		m_fnn.freeFNN();

	CDocument::OnCloseDocument();
}

void CSoftComputingWndDoc::OnLearnFmmnn() 
{
	if (filename.Find(".fmm") <= -1)
		return;

	int i, j;
	float **data;
	int *classInfo;
	int dataLength;
	int numInput, numClass;
	float Theta, Gamma;
	CString message, message2;
	FILE *fp;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();

	fp = fopen(filename, "rt");
	fscanf(fp, "%d %d %d %f %f", &dataLength, &numInput, &numClass, &Theta, &Gamma);
	
	m_fmmnn.Init(numInput, numClass, Theta, Gamma);

	data = new float*[dataLength];
	classInfo = new int[dataLength];

	for (i=0; i<dataLength; i++)
	{
		data[i] = new float[numInput];
		for (j=0; j<numInput; j++)			fscanf(fp, "%f", &(data[i][j]));
		fscanf(fp, "%d", &(classInfo[i]));
	}
	fclose(fp);


	//filename += ".out";
	//fp = fopen(filename, "wt");

	//for (k=0; k<10; k++)
	{
	//	m_fmmnn.SetTheta((double)((double)k*0.01f));

		(mainframe->GetLogView())->AddLine("TRAINING START");

		// training
		for (i=0; i<dataLength; i++)
			m_fmmnn.Training(classInfo[i], data[i]);

		(mainframe->GetLogView())->AddLine("TRAINING END");

		//fprintf(fp, "%.2f %d %.2f \n", m_fmmnn.GetTheta(), m_fmmnn.GetTotalBoxNum(), rr);
	}
	//fclose(fp);

	delete [] data;	data = NULL;
}

void CSoftComputingWndDoc::OnParamSetFnn() 
{
	FNNdialog FNNdlg;

	FNNdlg.m_learning_rate = m_fnn.learn_rate;
	FNNdlg.m_error_criteria = m_fnn.err_criteria;
	FNNdlg.m_epoch = m_fnn.epoch;

	FNNdlg.m_io_histogram = m_fnn.io_histogram_flag;
	FNNdlg.m_rule_combination = m_fnn.rule_combination_flag;
	FNNdlg.m_SaveRFR = m_fnn.save_rfr;
	FNNdlg.m_SaveSI = m_fnn.save_si;
	FNNdlg.m_KillNodes = m_fnn.kill_nodes;
	FNNdlg.m_JustInputs = m_fnn.just_inputs;

	FNNdlg.m_SLP1 = m_fnn.abs_sum_weight;
	FNNdlg.m_SLP2 = m_fnn.max_weight;

	if (FNNdlg.DoModal() == IDOK)
	{
		m_fnn.learn_rate = FNNdlg.m_learning_rate;
		m_fnn.err_criteria = FNNdlg.m_error_criteria;
		m_fnn.epoch = FNNdlg.m_epoch;

		m_fnn.io_histogram_flag = FNNdlg.m_io_histogram;
		m_fnn.rule_combination_flag = FNNdlg.m_rule_combination;
		m_fnn.save_rfr = FNNdlg.m_SaveRFR;
		m_fnn.save_si = FNNdlg.m_SaveSI;
		m_fnn.kill_nodes = FNNdlg.m_KillNodes;
		m_fnn.just_inputs = FNNdlg.m_JustInputs;

		FNNkillnodes = FNNdlg.m_strNodes;
		FNNjustinputs = FNNdlg.m_strInputs;

		m_fnn.abs_sum_weight = FNNdlg.m_SLP1;
		m_fnn.max_weight = FNNdlg.m_SLP2;
	}
}

void CSoftComputingWndDoc::OnLearnFnn() 
{
	if (!datLoaded)
		return;

	// prepare initial config.
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	m_fnn.imgview = mainframe->GetImageBaseView();
	m_fnn.imgview->SetImagePlane(1, 1);
	m_fnn.logview = mainframe->GetLogView();
	m_fnn.chartview[0] = mainframe->GetChart(0);
	m_fnn.chartview[1] = mainframe->GetChart(1);
	m_fnn.scrollchartview = mainframe->GetScrollChart();

	m_fnn.initFNN();
	m_fnn.setMembership();
	m_fnn.compFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);

	float clr;
	//m_fnn.recTrain(&clr);

	m_fnn.ruleCombination();

	m_fnn.ShowFNND();
	m_fnn.ioHistogram();
	//m_fnn.SaveIOHistogram(".\\myIOHisto.dat");

	//m_fnn.learnFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);
	m_fnn.learnFNN_Thread();

	//m_fnn.recTrain(&clr);
	//m_fnn.recTest(&clr);

	//m_fnn.ShowMF(0); 	m_fnn.ShowMF(1);
}

void CSoftComputingWndDoc::OnCamClose() 
{
	/*
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();
	
	camview->CameraOff();
	*/
}

void CSoftComputingWndDoc::OnCamFormat() 
{
	/*
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();
	
	camview->CameraFormat();
	*/
}

void CSoftComputingWndDoc::OnCamInit() 
{
	/*
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();

	camview->CameraOn();
	*/
}

void CSoftComputingWndDoc::OnTestFmmnn() 
{
	if (filename.Find(".bxd") <= -1)
		return;

	int i, j;
	float **data;
	int *classInfo;
	int dataLength;
	int numInput, numClass;
	float Theta, Gamma;
	int *cmat;
	float rr = 0.0f;
	CString message, message2;
	FILE *fp;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CFileDialog *pDlg = new CFileDialog( TRUE, NULL, NULL, OFN_READONLY, 
 		         "FMMNN파일 *.fmm|*.fmm|모든파일 *.*|*.*|", NULL );

	if (!pDlg->DoModal())
		return;

	fp = fopen(pDlg->GetFileName(), "rt");
	fscanf(fp, "%d %d %d %f %f", &dataLength, &numInput, &numClass, &Theta, &Gamma);
	
	m_fmmnn.Init(numInput, numClass, Theta, Gamma);

	data = new float*[dataLength];
	classInfo = new int[dataLength];

	cmat = new int[numClass*numClass];
	for (i=0; i<numClass; i++)
		for (j=0; j<numClass; j++)
			cmat[i*numClass + j] = 0;

	// data loading
	for (i=0; i<dataLength; i++)
	{
		data[i] = new float[numInput];
		for (j=0; j<numInput; j++)			fscanf(fp, "%f", &(data[i][j]));
		fscanf(fp, "%d", &(classInfo[i]));
	}
	fclose(fp);

	(mainframe->GetLogView())->AddLine("LOAD BOX");
	m_fmmnn.LoadBox2(filename);

	(mainframe->GetLogView())->AddLine("TESTING START");
	// test
	for (i=0; i<dataLength; i++)
		cmat[classInfo[i]*numClass + m_fmmnn.Test(data[i])]++;

	for (i=0; i<numClass; i++)
		rr += cmat[i*numClass + i];

	rr /= (float)dataLength;
	(mainframe->GetLogView())->AddLine("TESTING END");

	message.Format("\r\nRecognition Rate for Training Set [%.2f]", rr*100.0f);
	(mainframe->GetLogView())->AddLine(message);

	message.Format("");
	for (i=0; i<numClass; i++)
	{
		message2.Format("%d ", cmat[i*numClass+i]);
		message += message2;
	}
	(mainframe->GetLogView())->AddLine(message);

	delete [] cmat; cmat = NULL;
	delete [] data;	data = NULL;
}

void CSoftComputingWndDoc::OnCamSetting() 
{
	/*
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();

	camview->CameraSetting();
	*/
}

// 20030211
void CSoftComputingWndDoc::OnEditEtc() 
{
	int i, j, k, val;
	CString filename;

	/*
	IPLIMAGE TimageC = cvvLoadImage(".\\CHATanal\\TestImage3.jpg");
	IPLIMAGE TimageG = cvCreateImage(cvSize(960,720), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageC = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 3);
	IPLIMAGE SimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);

	cvvConvertImage(TimageC, TimageG, 0);

	k = 0;
	for (i=0; i<6; i++)
		for (j=0; j<6; j++)
		{
			cvSetImageROI(TimageG, cvRect(160*i,120*j,160,120));
			iplCopy(TimageG, SimageG);
			cvvConvertImage(SimageG, SimageC, 0);
			filename.Format(".\\CHATanal\\CHATimgG%03d.jpg", k);
			cvvSaveImage(filename, SimageC);

			k += 10;
		}

	cvReleaseImage(&TimageG);
	cvReleaseImage(&SimageG);
	cvReleaseImage(&SimageC);	
	*/

	#define	CE21	(2147483648)

	FILE *fp, *fp2, *fp3, *fp4, *fp5, *fp6;
	fp = fopen(".\\CHATperformance.dat", "wt");
	fp2 = fopen(".\\CHAT_I2.dat", "wt");
	fp3 = fopen(".\\CHAT_I3.dat", "wt");
	fp4 = fopen(".\\CHAT_H.dat", "wt");
	fp5 = fopen(".\\CHAT_L.dat", "wt");
	fp6 = fopen(".\\CHAT_S.dat", "wt");

	float sHit, seHit, nseHit, nsHit, tPix, sPix, nPix;
	float C, S, SE, NSE;
	int bins = 256;
	int dims1D[1] = {bins};
	float *hist_th = new float[2];
	hist_th[0] = 0.0f; hist_th[1] = 255.0f;
	CvHistogram* histG = new CvHistogram;
	histG = cvCreateHist(1, dims1D, CV_HIST_ARRAY);
	cvSetHistThresh(histG, &hist_th, 1);
	delete [] hist_th;

	int twistdata[16] = {
		0.33*CE21, 0.33*CE21, 0.33*CE21, 0, 
		-0.5*CE21, 0, 0.5*CE21, 0, 
		-0.25*CE21, 0.5*CE21, -0.25*CE21, 0,
		0, 0, 0, CE21};

	IplColorTwist *mytwist = iplCreateColorTwist(twistdata, 31);
	iplSetColorTwist(mytwist, twistdata, 31);

	tPix = 160*120;

	IPLIMAGE SimageCT = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 3);
	IPLIMAGE SimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE GimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE TimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE TimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE GimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE NSimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE NGimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);

	IPLIMAGE SimageC1 = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageC2 = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageC3 = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);

	for (i=0; i<59; i++)
	{		
		filename.Format(".\\CHATanal\\DJface%03d.bmp", i*10);
		IPLIMAGE SimageC = cvvLoadImage(filename);
		iplSet(SimageG, 0);
		cvvConvertImage(SimageC, SimageG);
		iplSet(SimageB, 0);
		iplThreshold(SimageG, SimageB, 1);
		iplSet(NSimageB, 0);
		iplNot(SimageB, NSimageB);

		iplRGB2HLS(SimageC, SimageCT);
		cvCvtPixToPlane(SimageCT, SimageC1, SimageC2, SimageC3, 0);

		cvCalcHist(&SimageC1, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp4, "%d ", (int)val);
		}
		fprintf(fp4, "\n");

		cvCalcHist(&SimageC2, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp5, "%d ", (int)val);
		}
		fprintf(fp5, "\n");
		
		cvCalcHist(&SimageC3, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp6, "%d ", (int)val);
		}
		fprintf(fp6, "\n");


		iplApplyColorTwist(SimageC, SimageCT, mytwist, 0);
		cvCvtPixToPlane(SimageCT, SimageC1, SimageC2, SimageC3, 0);

		cvCalcHist(&SimageC2, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp2, "%d ", (int)val);
		}
		fprintf(fp2, "\n");
		
		cvCalcHist(&SimageC3, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp3, "%d ", (int)val);
		}
		fprintf(fp3, "\n");

		/*
				for (i = 1; i < bins; i++) {
			val = cvRound(cvQueryHistValue_1D( histG, i ));
			if (val > MaxVal)
				MaxVal = val;
		}
*/
		
		filename.Format(".\\CHATanal\\CHATimgG%03d.jpg", i*10);
		IPLIMAGE GimageC = cvvLoadImage(filename);
		iplSet(GimageG, 0);
		cvvConvertImage(GimageC, GimageG);
		iplSet(GimageB, 0);
		iplThreshold(GimageG, GimageB, 1);
		iplSet(NGimageB, 0);
		iplNot(GimageB, NGimageB);

		sPix = (float)cvCountNonZero(GimageG);
		nPix = tPix - sPix;

		iplSet(TimageB, 0);
		iplAnd(SimageB, GimageB, TimageB);
		sHit = (float)cvCountNonZero(TimageB);

		iplSet(TimageB, 0);
		iplAnd(NSimageB, GimageB, TimageB);
		seHit = (float)cvCountNonZero(TimageB);

		iplSet(TimageB, 0);
		iplAnd(SimageB, NGimageB, TimageB);
		nseHit = (float)cvCountNonZero(TimageB);

		iplSet(TimageB, 0);
		iplAnd(NSimageB, NGimageB, TimageB);
		nsHit = (float)cvCountNonZero(TimageB);

		C = (sHit+nsHit)/tPix;
		SE = seHit/tPix;
		NSE = nseHit/tPix;
		S = sHit/sPix;

		fprintf(fp, "%.2f %.2f %.2f %.2f %.0f %.0f %.0f %.0f %.0f \n", C, S, SE, NSE, sPix, sHit, seHit, nseHit, nsHit);
	}

	cvReleaseImage(&SimageC3);
	cvReleaseImage(&SimageC2);	
	cvReleaseImage(&SimageC1);

	cvReleaseImage(&NGimageB);
	cvReleaseImage(&NSimageB);	
	cvReleaseImage(&GimageB);
	cvReleaseImage(&SimageB);	
	cvReleaseImage(&TimageG);
	cvReleaseImage(&TimageB);
	cvReleaseImage(&GimageG);
	cvReleaseImage(&SimageG);	
	cvReleaseImage(&SimageCT);	

	iplDeleteColorTwist(mytwist);

	cvReleaseHist(&histG);

	fclose(fp6);
	fclose(fp5);
	fclose(fp4);
	fclose(fp3);
	fclose(fp2);
	fclose(fp);
}

//20030613
void CSoftComputingWndDoc::OnLearnFnn2() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	m_fnn.imgview = mainframe->GetImageBaseView();
	m_fnn.imgview->SetImagePlane(5, 1);
	m_fnn.logview = mainframe->GetLogView();

	// parsing
	char seps[] = " ";
	char *token;
	int no_nodes = 0;
	int* nodes = new int[m_fnn.layer[4]];
	int n;

	token = strtok((char*)(LPCTSTR)FNNjustinputs, seps);

	while (token != NULL)
	{
		nodes[no_nodes++] = atoi(token);
		token = strtok( NULL, seps);
	}

	// ???
	m_fnn.DataTransform(nodes, no_nodes);
	//m_fnn.layer[4] = no_nodes;
	//m_fnn.layer[3] = no_nodes*2;
	// ???

	m_fnn.initFNN();
	m_fnn.setMembership();
	m_fnn.compFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);

			float clr;
			m_fnn.recTrain(&clr);

	m_fnn.ruleCombination();
	m_fnn.ioHistogram();
	m_fnn.SaveIOHistogram(".\\myIOHisto.dat");

	//m_fnn.learnFNN2(m_fnn.lin, m_fnn.lout, m_fnn.l_no, nodes, no_nodes);
	m_fnn.learnFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);

			m_fnn.recTrain(&clr);
			m_fnn.recTest(&clr);

	m_fnn.ShowMF(0); 	m_fnn.ShowMF(1);
	m_fnn.ShowFNND();

	delete [] nodes;
}

void CSoftComputingWndDoc::OnLearnIfnn() 
{
	if (!datLoaded)
		return;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	m_fnn.imgview = mainframe->GetImageBaseView();
	m_fnn.imgview->SetImagePlane(5, 1);
	m_fnn.logview = mainframe->GetLogView();

	m_fnn.initFNN();
	m_fnn.setMembership();
	m_fnn.compFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);

			float clr;
			m_fnn.recTrain(&clr);

	m_fnn.ruleCombination();
	//m_fnn.ioHistogram();
	//m_fnn.SaveIOHistogram(".\\myIOHisto.dat");

	m_fnn.learnFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);

			m_fnn.recTrain(&clr);
			m_fnn.recTest(&clr);

	m_fnn.ShowMF(0); 	m_fnn.ShowMF(1);
	m_fnn.ShowFNND();
}

void CSoftComputingWndDoc::OnFsFastComparison() 
{
	int i, j;
	float clr = 0.0f;
	FILE *fp;

	fp = fopen(".\\FS\\FSresult040217.dat", "awt");

	char *fnns[5] = {".\\FS\\exjsyfN5.fnn", ".\\FS\\exktsfN5.fnn", ".\\FS\\exkyjfN5.fnn", ".\\FS\\exkym2fN5.fnn", ".\\FS\\exljwfN5.fnn"};
	char *dats[5] = {".\\FS\\exjsyfN5.dat", ".\\FS\\exktsfN5.dat", ".\\FS\\exkyjfN5.dat", ".\\FS\\exkym2fN5.dat", ".\\FS\\exljwfN5.dat"};

	/*
	char *strA[42] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", //10
		"2 3", "6 7", "8 9", "0 1", "4 5", "0 8", "0 9", "1 8", "1 9", "4 8", "4 9", "5 8", "5 9", "6 8", "6 9", "7 8", "7 9", //17
		"0 1 8", "0 1 9", "0 8 9", "1 8 9", "4 5 8", "4 5 9", "4 8 9", "5 8 9", "6 8 9", "7 8 9", "6 7 8", "6 7 9", //12
	"0 1 8 9", "4 5 8 9", "6 7 8 9"}; //3
	*/
	char *strA[9] = {"0 4", "0 5", "1 4", "1 5", //6
		"0 4 5", "1 4 5", "0 1 4", "0 1 5", //6
		"0 1 4 5" //3
	};

	// A, C, GS, MF2, NR, WF2
	for (i=0; i<5; i++)
	{
		for (j=0; j<9; j++)
		{
		myFNN onlyFNN;

		onlyFNN.kill_nodes = 1;

		onlyFNN.loadFNN(fnns[i]);

		// parsing
		char seps[] = " ";
		char *token;
		int no_nodes = 0;
		int* nodes = new int[onlyFNN.layer[1]];
		int n;

		CString KillNodes;
		KillNodes.Format("%s", strA[j]);
		token = strtok((char*)(LPCTSTR)KillNodes, seps);

		while (token != NULL)
		{
			nodes[no_nodes++] = atoi(token);
			token = strtok( NULL, seps);
		}

	    onlyFNN.loadDataN1(dats[i]);

		onlyFNN.initFNN();

		onlyFNN.deleteInputNodes(nodes, no_nodes);

		onlyFNN.recTest(&clr);

		fprintf(fp, "(%s) %.2f ", strA[j], clr);

		delete [] nodes;		

		}

		fprintf(fp, "\n");
	}

	fclose(fp);
}



void CSoftComputingWndDoc::OnLearnFex() 
{
	CIplFace face;

	IPLIMAGE init_img = cvLoadImage("K:\\User\\research\\faceDB\\ZBienDB\\kts1\\kts10212161141_happy.jpg");

	face.iplFaceAllocImage(init_img);

	face.ShowFace(NULL, NULL);

	face.iplFaceFreeImage();
}

void CSoftComputingWndDoc::OnLearnStopThread() 
{
	m_fnn.break_flag = TRUE;
}
