/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



// SproutViewer.h : SproutViewer �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��


// CSproutViewerApp:
// ���̃N���X�̎����ɂ��ẮASproutViewer.cpp ���Q�Ƃ��Ă��������B
//

class CSproutViewerApp : public CWinApp
{
	bool m_bAllWindowInitialized;

public:
	CSproutViewerApp();


// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSproutViewerApp theApp;
