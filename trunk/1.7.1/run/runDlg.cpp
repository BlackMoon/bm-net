// runDlg.cpp : implementation file
//

#include "stdafx.h"
#include "run.h"
#include "runDlg.h"
#include ".\rundlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CrunDlg dialog



CrunDlg::CrunDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CrunDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CoInitialize(0);
}
CrunDlg::~CrunDlg()
{
	//app.ReleaseDispatch();
	if (gr.m_lpDispatch) gr.ReleaseDispatch();
	if (app.m_lpDispatch) app.ReleaseDispatch();
	if (atl.m_lpDispatch) atl.ReleaseDispatch();
	CoUninitialize();
}

void CrunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CrunDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()


// CrunDlg message handlers

BOOL CrunDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CrunDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CrunDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CrunDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CrunDlg::OnBnClickedButton1()
{
	atl.CreateDispatch("atl.Document");
	atl.show();

}

void CrunDlg::OnBnClickedButton2()
{
	if (app.m_lpDispatch == 0) 
	{
		app.CreateDispatch("Model.Application");			
	}
	VARIANT b = app.GetWellCharsetFont();
	CString s1 = app.GetWellFacenameFont();
	app.SetWellR1(100);


	SAFEARRAY* psa = SafeArrayCreateVector(VT_I4, 0, 3);	
	DWORD* pvdata = (DWORD*)psa->pvData;	
	pvdata[0] = 0xff000000 | (255 & 0xff) << 16;// | (0 & 0xff) << 8 | 0 & 0xff;
	pvdata[1] = 0xff000000 | (0 & 0xff) << 16 | (255 & 0xff) << 8 | 0 & 0xff;
	pvdata[2] = 0xff000000 | (0 & 0xff) << 16 | (0 & 0xff) << 8 | 255 & 0xff;
	
	
		
	COleVariant olearr;
	olearr.vt = VT_ARRAY | VT_I4;
	olearr.parray = psa;
	SCODE s2 = app.SetLegend(olearr);
	
	

	app.Show();

	
	BOOL b1 = 0;
	b1 = app.GetAuto();
		//b1 = app.GetLegendInv();
	
	app.SetAuto(0);
	
	CString s = "d:\\Geology\\dxpr\\tnk\\botv_bb\\botv_bb.mvf";
	app.Open(s, 0);

	LPDISPATCH pGraph = app.GetGraph();
	if (pGraph)
	{
		gr.AttachDispatch(pGraph);		
		gr.LeftView();
		
		gr.ShowCCW(0, 1, 1);
		gr.SetDims(1L, 4L, 0L, 100L, 2L, 12L);
	}	
	
	
	Catl0 atl0;
	atl0.m1(1L);
	

}
void CrunDlg::OnBnClickedButton3()
{
	gr.TopView();
	gr.ResetFilter();
	CString s = "d:\\1.jpg";
	COleVariant v = COleVariant(1L);
	app.Save(s, COleVariant(1L));//, COleVariant(1L));
}
