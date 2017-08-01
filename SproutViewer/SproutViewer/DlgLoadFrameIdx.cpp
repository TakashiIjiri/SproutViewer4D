/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



// DlgLoadFrameIdx.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SproutViewer.h"
#include "DlgLoadFrameIdx.h"
#include "afxdialogex.h"


// DlgLoadFrameIdx ダイアログ

IMPLEMENT_DYNAMIC(DlgLoadFrameIdx, CDialogEx)

DlgLoadFrameIdx::DlgLoadFrameIdx(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SETFRAMESIZE, pParent)
{
	m_bInitialized = false;

}

DlgLoadFrameIdx::~DlgLoadFrameIdx()
{
}

void DlgLoadFrameIdx::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_LDFRM_STARTI, m_spin_startI);
	DDX_Control(pDX, IDC_SPIN_LDFRM_ENDI  , m_spin_endI);
}


BEGIN_MESSAGE_MAP(DlgLoadFrameIdx, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_LDFRM_STARTI, &DlgLoadFrameIdx::OnEnChangeEditLdfrmStarti)
	ON_EN_CHANGE(IDC_EDIT_LDFRM_ENDI  , &DlgLoadFrameIdx::OnEnChangeEditLdfrmEndi)
END_MESSAGE_MAP()


// DlgLoadFrameIdx メッセージ ハンドラー


BOOL DlgLoadFrameIdx::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_spin_startI.SetRange32(0, m_availFrameN-1);
	m_spin_endI  .SetRange32(0, m_availFrameN-1);
	m_spin_startI.SetPos32( 0 );
	m_spin_endI  .SetPos32( m_availFrameN-1 );

	m_tmpStartI = m_spin_startI.GetPos32();
	m_tmpEndI   = m_spin_endI  .GetPos32();

	m_bInitialized = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void DlgLoadFrameIdx::OnEnChangeEditLdfrmStarti()
{
	if( !m_bInitialized ) return;
	m_tmpStartI = m_spin_startI.GetPos32();
	m_tmpEndI   = m_spin_endI  .GetPos32();
}


void DlgLoadFrameIdx::OnEnChangeEditLdfrmEndi()
{
	if( !m_bInitialized ) return;
	m_tmpStartI = m_spin_startI.GetPos32();
	m_tmpEndI   = m_spin_endI  .GetPos32();
}
