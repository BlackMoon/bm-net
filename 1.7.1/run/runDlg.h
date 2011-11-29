// runDlg.h : header file
//
#include "CApplication.h"
#include "CGraph.h"
#include "Catl.h"
#include "Catl0.h"

#pragma once


// CrunDlg dialog
class CrunDlg : public CDialog
{
// Construction
public:
	CrunDlg(CWnd* pParent = NULL);	// standard constructor
	~CrunDlg();
// Dialog Data
	enum { IDD = IDD_RUN_DIALOG };	
	CApplication app;
	Catl atl;
	CGraph gr;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
