/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/


#pragma once
#include "afxcmn.h"


// DlgLoadFrameIdx ダイアログ

class DlgLoadFrameIdx : public CDialogEx
{
	DECLARE_DYNAMIC(DlgLoadFrameIdx)


	bool m_bInitialized;
	int  m_availFrameN;

	int  m_tmpStartI, m_tmpEndI;

public:
	DlgLoadFrameIdx(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~DlgLoadFrameIdx();

	INT_PTR myDoModal( int frameN, int &startI, int &endI )
	{
		m_availFrameN = frameN;
		INT_PTR res = DoModal();

		startI = m_tmpStartI;
		endI   = m_tmpEndI  ;
		return res;
	}


// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETFRAMESIZE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spin_startI;
	CSpinButtonCtrl m_spin_endI;
	afx_msg void OnEnChangeEditLdfrmStarti();
	afx_msg void OnEnChangeEditLdfrmEndi();
};
