/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/




// SproutViewerView.h : CSproutViewerView クラスのインターフェイス
//

#pragma once

#include "COMMON/OglForMFC.h"
#include "SproutViewerDoc.h"

class CSproutViewerView : public CView
{
	CString m_topDir;
	OglForMFC m_ogl;
	CString BrowseForFolder( HWND p_hWnd, CString p_cSetStr, CString p_cRootStr, CString p_cCaptionStr, UINT p_uiFlags );

	static CSproutViewerView *m_myself;

protected: // シリアル化からのみ作成します。
	CSproutViewerView();
	DECLARE_DYNCREATE(CSproutViewerView)

// 属性
public:
	CSproutViewerDoc* GetDocument() const;

// 操作
public:
	void load4dct();
	void initCameraPos(EVec3f &cuboid){ AfxMessageBox("to implement");}
	static CSproutViewerView* getInst(){ return m_myself; }

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 実装
public:
	virtual ~CSproutViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp  (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove    (UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown  (UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp    (UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown  (UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp    (UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown  (UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp    (UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // SproutViewerView.cpp のデバッグ バージョン
inline CSproutViewerDoc* CSproutViewerView::GetDocument() const
   { return reinterpret_cast<CSproutViewerDoc*>(m_pDocument); }
#endif

