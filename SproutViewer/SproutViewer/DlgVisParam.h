/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



#pragma once

#include "COMMON/tmath.h"
#include "afxcmn.h"
#include "afxwin.h"
// DlgVisParam ダイアログ

class DlgVisParam : public CDialogEx
{
	DECLARE_DYNAMIC(DlgVisParam)

	bool   m_drawTf    ;
	int    m_drawTfCh  ; //0 or 1
	CPoint m_preP      ;
	byte  *m_pcBgBits1, *m_pcBgBits2;
	EVec4i m_pcRect1  ,  m_pcRect2  ;

	bool m_bInitialized;


	void New4DCTLoaded(const int frameN, const EVec3i reso, const EVec3f pitch, const EVec2i WinLv);
	void HistogramUpdated(const int N, const float *histI, const float* histG);


public:
	DlgVisParam(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~DlgVisParam();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VISPARAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_slider_timeI;
	CSliderCtrl m_slider_sliceN;
	CSliderCtrl m_slider_transAlpha;
	CSliderCtrl m_slider_winLvMin;
	CSliderCtrl m_slider_winLvMax;
	CEdit		m_edit_timeI;
	CEdit		m_edit_WinLvMin;
	CEdit		m_edit_WinLvMax;
	CStatic m_pc1;
	CStatic m_pc2;
	CButton m_check_planeXY;
	CButton m_check_planeYZ;
	CButton m_check_planeZX;
	CButton m_check_bgWhite;
	CButton m_check_bgBlack;
	CButton m_check_frame  ;
	CButton m_check_volume ;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNMReleasedcaptureSliderWinlvMin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderWinlvMax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp  (UINT nFlags, CPoint point);
	afx_msg void OnMouseMove  (UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCheckBkWhite();
	afx_msg void OnBnClickedCheckBkBlack();
	afx_msg void OnPaint();


	float getTransAlpha();
};
