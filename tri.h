// tri.h
#pragma once
#pragma comment(lib, "../hardlock/hlw32_mc")
#pragma comment(lib, "../html/htmlhelp")

#include "frame.h"
#include "graph.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define MODAD	24042
// CtriApp:
class CtriApp : public CWinApp
{
private:
	bool hardlock();	
	bool loadRegSettings();				
	void saveRegSettings();
public:
	CtriApp();	
	int ExitInstance();
	BOOL InitInstance();
	BOOL OnIdle(LONG lCount);	
	BOOL ProcessShellCommand(CCommandLineInfo& rCmdInfo);
	CDocument* OpenDocumentFile(LPCTSTR lpszFileName);	
	void main();
	void OnFileNew();
	void saveRegColors();
	~CtriApp();
// Attributes
public:
	storage					stg;
	CGraph					m_graph;
	COleTemplateServer		m_server;
	LPTSTR					lpszPath;	
	CMainFrame*				m_pFrame;
protected:
	DECLARE_MESSAGE_MAP()	
	afx_msg void OnChangeOption();
	afx_msg void OnHelp();	
	afx_msg void OnFileOpen();			
	afx_msg void OnStat();	
	afx_msg void OnUpdateStat(CCmdUI* pCmdUI);		
};
extern CtriApp app;