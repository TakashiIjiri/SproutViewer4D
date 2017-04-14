/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



// DlgVisParam.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SproutViewer.h"
#include "DlgVisParam.h"
#include "afxdialogex.h"

#include "SproutViewerView.h"
#include "ImageManager.h"



static double SLIDER_RATE = 10;

// DlgVisParam ダイアログ

IMPLEMENT_DYNAMIC(DlgVisParam, CDialogEx)

DlgVisParam::DlgVisParam(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_VISPARAM, pParent)
{
	m_bInitialized = false;

}

DlgVisParam::~DlgVisParam()
{
}

void DlgVisParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_TIMELINE, m_slider_timeI);
	DDX_Control(pDX, IDC_SLIDER_SLICE_NUM, m_slider_sliceN);
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_slider_transAlpha);
	DDX_Control(pDX, IDC_SLIDER_WINLV_MIN, m_slider_winLvMin);
	DDX_Control(pDX, IDC_SLIDER_WINLV_MAX, m_slider_winLvMax);


	DDX_Control(pDX, IDC_PC_TRANSFUNC1 , m_pc1);
	DDX_Control(pDX, IDC_PC_TRANSFUNC2 , m_pc2);
	DDX_Control(pDX, IDC_CHECK_PLANE_XY, m_check_planeXY);
	DDX_Control(pDX, IDC_CHECK_PLANE_YZ, m_check_planeYZ);
	DDX_Control(pDX, IDC_CHECK_PLANE_ZX, m_check_planeZX);
	DDX_Control(pDX, IDC_CHECK_BK_WHITE, m_check_bgWhite);
	DDX_Control(pDX, IDC_CHECK_BK_BLACK, m_check_bgBlack);
	DDX_Control(pDX, IDC_CHECK_FRAME   , m_check_frame);
	DDX_Control(pDX, IDC_CHECK_VOLUME  , m_check_volume);
	DDX_Control(pDX, IDC_EDIT_WINLV_MIN, m_edit_WinLvMin);
	DDX_Control(pDX, IDC_EDIT_WINLV_MAX, m_edit_WinLvMax);
	DDX_Control(pDX, IDC_EDIT_TIMEI    , m_edit_timeI);
	DDX_Control(pDX, IDC_CHECK_VIS_MASK, m_check_Mask);
	DDX_Control(pDX, IDC_CHECK_VIS_SURF, m_check_surf);
}


BEGIN_MESSAGE_MAP(DlgVisParam, CDialogEx)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_WINLV_MIN, &DlgVisParam::OnNMReleasedcaptureSliderWinlvMin)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_WINLV_MAX, &DlgVisParam::OnNMReleasedcaptureSliderWinlvMax)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CHECK_BK_WHITE, &DlgVisParam::OnBnClickedCheckBkWhite)
	ON_BN_CLICKED(IDC_CHECK_BK_BLACK, &DlgVisParam::OnBnClickedCheckBkBlack)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_FRAME, &DlgVisParam::OnBnClickedCheckFrame)
	ON_BN_CLICKED(IDC_CHECK_VOLUME, &DlgVisParam::OnBnClickedCheckVolume)
	ON_BN_CLICKED(IDC_CHECK_PLANE_XY, &DlgVisParam::OnBnClickedCheckPlaneXy)
	ON_BN_CLICKED(IDC_CHECK_PLANE_YZ, &DlgVisParam::OnBnClickedCheckPlaneYz)
	ON_BN_CLICKED(IDC_CHECK_PLANE_ZX, &DlgVisParam::OnBnClickedCheckPlaneZx)
	ON_BN_CLICKED(IDC_CHECK_VIS_SURF, &DlgVisParam::OnBnClickedCheckVisSurf)
	ON_BN_CLICKED(IDC_CHECK_VIS_MASK, &DlgVisParam::OnBnClickedCheckVisMask)
	ON_BN_CLICKED(IDC_BUTTON_COMPUTE_SURF, &DlgVisParam::OnBnClickedButtonComputeSurf)
	ON_BN_CLICKED(IDC_BUTTON_COMPUTE_MASK, &DlgVisParam::OnBnClickedButtonComputeMask)
END_MESSAGE_MAP()


// DlgVisParam メッセージ ハンドラー



void DlgVisParam::HistogramUpdated(const int N, const float *histI, const float* histG)
{
	const int w1 = m_pcRect1[2];
	const int h1 = m_pcRect1[3];
	const int w2 = m_pcRect2[2];
	const int h2 = m_pcRect2[3];

	//intensity
	for (int y = 0; y < h1; ++y)
	{
		for (int x = 0; x < w1; ++x)
		{
			const int I = 4*( x + y * w1);
			float thresh = histI[(int)( (x / (double)w1) * N)];

			byte val = (y / (float)h1) < thresh ? 64 : 0;
			m_pcBgBits1[I] = m_pcBgBits1[I+1] = m_pcBgBits1[I+2] = m_pcBgBits1[I+3] = val;
		}
	}

	//gradient 
	for (int y = 0; y < h2; ++y)
	{
		for (int x = 0; x < w2; ++x)
		{
			const int I = 4*( x + y * w2);
			float thresh = histG[(int)( (x / (double)w2) * N)];

			byte val = (y / (float)h2) < thresh ? 64: 0;
			m_pcBgBits2[I] = m_pcBgBits2[I+1] = m_pcBgBits2[I+2] = m_pcBgBits2[I+3] = val;
		}
	}
}







void DlgVisParam::New4DCTLoaded(const int frameN, const EVec3i reso, const EVec3f pitch, const EVec2i WinLv)
{
	m_bInitialized = false;
	CString str;

	/*
	str.Format("%d"  , reso[0] ); m_edit_SizeW.SetWindowTextA(str);
	str.Format("%d"  , reso[1] ); m_edit_SizeH.SetWindowTextA(str);
	str.Format("%d"  , reso[2] ); m_edit_SizeD.SetWindowTextA(str);
	str.Format("%.3f", pitch[0]); m_edit_PitchW.SetWindowTextA(str);
	str.Format("%.3f", pitch[1]); m_edit_PitchH.SetWindowTextA(str);
	str.Format("%.3f", pitch[2]); m_edit_PitchD.SetWindowTextA(str);
	*/
	m_slider_timeI   .SetRange(0, frameN-1);
	m_slider_winLvMin.SetRange((int)(WinLv[0] * SLIDER_RATE), (int)(WinLv[1] * SLIDER_RATE));
	m_slider_winLvMax.SetRange((int)(WinLv[0] * SLIDER_RATE), (int)(WinLv[1] * SLIDER_RATE));

	m_slider_timeI   .SetPos  ( 0 );
	m_slider_winLvMin.SetPos  ((int)(WinLv[0] * SLIDER_RATE));
	m_slider_winLvMax.SetPos  ((int)(WinLv[1] * SLIDER_RATE));

	double c = 1.0 / SLIDER_RATE;
	str.Format("%.1f", c * m_slider_winLvMin.GetPos()); m_edit_WinLvMin.SetWindowTextA(str);
	str.Format("%.1f", c * m_slider_winLvMax.GetPos()); m_edit_WinLvMax.SetWindowTextA(str);
	str.Format("%d"  , m_slider_timeI.GetPos()       ); m_edit_timeI   .SetWindowTextA(str);

	m_bInitialized = true;
}



/*

static bool t_readEditAsDouble(CEdit *edit, double &val)
{
	char buf[1024], *e;
	edit->GetWindowTextA(buf, 1024);
	double d = strtod(buf, &e);

	if (CString::StringLength(e) == 0 )
	{
		val = d;
		return true;
	}
	return false;
}




void DlgVolInfo::OnEnChangeEditViPitchW()
{
	if (!m_bInitialized) return;

	double d;
	if (t_readEditAsDouble( &m_edit_PitchW, d) && d > 0.001)
	{
		TexCore::getInst()->setPitchW( (float) d);
	}
	else
	{
		//strange imput (set prev value)
		CString str;
		str.Format("%.3f", TexCore::getInst()->getPitchW());
		m_edit_PitchW.SetWindowTextA(str);
	}
	CRoiPaintView::MyRedrawWindow();
}


void DlgVolInfo::OnEnChangeEditViPitchH()
{
	if (!m_bInitialized) return;

	double d;
	if (t_readEditAsDouble(&m_edit_PitchH, d) && d > 0.001)
	{
		TexCore::getInst()->setPitchH((float)d);
	}
	else
	{
		//strange imput (set prev value)
		CString str;
		str.Format("%.3f", TexCore::getInst()->getPitchH());
		m_edit_PitchH.SetWindowTextA(str);
	}
	CRoiPaintView::MyRedrawWindow();
}


void DlgVolInfo::OnEnChangeEditViPitchD()
{
	if (!m_bInitialized) return;

	double d;
	if (t_readEditAsDouble(&m_edit_PitchD, d) && d > 0.001)
	{
		TexCore::getInst()->setPitchD((float)d);
	}
	else
	{
		//strange imput (set prev value)
		CString str;
		str.Format("%.3f", TexCore::getInst()->getPitchD());
		m_edit_PitchD.SetWindowTextA(str);
	}
	CRoiPaintView::MyRedrawWindow();
}



void DlgVolInfo::OnBnClickedCheckViFrame   (){ CRoiPaintView::MyRedrawWindow(); }
void DlgVolInfo::OnBnClickedCheckViVolume  (){ CRoiPaintView::MyRedrawWindow(); }
void DlgVolInfo::OnBnClickedCheckViPlanexy (){ CRoiPaintView::MyRedrawWindow(); }
void DlgVolInfo::OnBnClickedCheckViPlaneyz (){ CRoiPaintView::MyRedrawWindow(); }
void DlgVolInfo::OnBnClickedCheckViPlanezx (){ CRoiPaintView::MyRedrawWindow(); }
void DlgVolInfo::OnBnClickedCheckViDopsuedo(){ CRoiPaintView::MyRedrawWindow(); }
void DlgVolInfo::OnBnClickedCheckViGradmag (){ CRoiPaintView::MyRedrawWindow(); }


void DlgVolInfo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRoiPaintView::getInst()->OnKeyDown(nChar, nRepCnt, nFlags);
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



*/

BOOL DlgVisParam::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CDialogEx::OnInitDialog();


	//disable close button
	CMenu*  pMenu = GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);

	WINDOWPLACEMENT w;
	m_pc1.GetWindowPlacement(&w);
	m_pcRect1 << w.rcNormalPosition.left, w.rcNormalPosition.top, w.rcNormalPosition.right - w.rcNormalPosition.left, w.rcNormalPosition.bottom - w.rcNormalPosition.top;
	m_pc2.GetWindowPlacement(&w);
	m_pcRect2 << w.rcNormalPosition.left, w.rcNormalPosition.top, w.rcNormalPosition.right - w.rcNormalPosition.left, w.rcNormalPosition.bottom - w.rcNormalPosition.top;
	m_pcBgBits1 = new byte[4 * m_pcRect1[2] * m_pcRect1[3]];
	m_pcBgBits2 = new byte[4 * m_pcRect2[2] * m_pcRect2[3]];


	m_slider_sliceN    .SetRange(32, 1024);
	m_slider_sliceN    .SetPos  (400     );
	m_slider_transAlpha.SetRange(0, 100  );
	m_slider_transAlpha.SetPos  (50      );


	m_check_frame   .SetCheck(1);
	m_check_volume  .SetCheck(1);
	m_check_Mask    .SetCheck(0);
	m_check_planeXY .SetCheck(1);
	m_check_planeYZ .SetCheck(1);
	m_check_planeZX .SetCheck(1);
	m_check_bgWhite .SetCheck(0);
	m_check_bgBlack .SetCheck(1);

	m_bInitialized = true;
	return TRUE;  
}


BOOL DlgVisParam::PreTranslateMessage(MSG* pMsg)
{

	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void DlgVisParam::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (*pScrollBar == m_slider_timeI)
	{
		CString str;
		str.Format("%d", m_slider_timeI.GetPos());
		m_edit_timeI.SetWindowTextA(str);

		
		double c = 1.0 / SLIDER_RATE;
		ImageManager::getInst()->updateVisVolume( (float)(m_slider_winLvMin.GetPos() * c), 
			                                      (float)(m_slider_winLvMax.GetPos() * c), m_slider_timeI.GetPos() );
		CSproutViewerView::getInst()->RedrawWindow();
	}
	if (*pScrollBar == m_slider_winLvMin)
	{
		CString str;
		double c = 1.0 / SLIDER_RATE;
		str.Format("%.1f", c * m_slider_winLvMin.GetPos()); 
		m_edit_WinLvMin.SetWindowTextA(str);
	}
	if(  *pScrollBar == m_slider_winLvMax)
	{
		CString str;
		double c = 1.0 / SLIDER_RATE;
		str.Format("%.1f", c * m_slider_winLvMax.GetPos()); 
		m_edit_WinLvMax.SetWindowTextA(str);
	}

	if (*pScrollBar == m_slider_transAlpha || *pScrollBar == m_slider_sliceN )
	{
		CSproutViewerView::getInst()->RedrawWindow();
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


float DlgVisParam::getTransAlpha()
{
	return 0.01f * m_slider_transAlpha.GetPos();;
}






void DlgVisParam::OnNMReleasedcaptureSliderWinlvMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	double c = 1.0 / SLIDER_RATE;
	ImageManager::getInst()->UpdateWindowLevel( (float)(m_slider_winLvMin.GetPos() * c), (float)(m_slider_winLvMax.GetPos() * c) );
	CSproutViewerView::getInst()->RedrawWindow();
	Invalidate(FALSE);
	UpdateWindow();
	*pResult = 0;
}


void DlgVisParam::OnNMReleasedcaptureSliderWinlvMax(NMHDR *pNMHDR, LRESULT *pResult)
{
	double c = 1.0 / SLIDER_RATE;
	ImageManager::getInst()->UpdateWindowLevel( (float)(m_slider_winLvMin.GetPos() * c), (float)(m_slider_winLvMax.GetPos() * c) );
	CSproutViewerView::getInst()->RedrawWindow();
	Invalidate(FALSE);
	UpdateWindow();
	*pResult = 0;
}




















//////////////////////////////////////////////////////////////////////////
//Transfer function///////////////////////////////////////////////////////


static void tf_draw(
	const int ch, 
	const int W , 
	const int H , 
	const OglImage1D<CH_RGBA> &tf, 
	CDC *dc)
{
	dc->MoveTo(0, (int)(H - H * tf[0+ch] / 255.0));

	double rate = (double) W / (TRANS_FUNC_SIZE - 1.0);

	for (int i = 1; i < TRANS_FUNC_SIZE; ++i)
	{
		dc->LineTo((int)(i * rate),
		(int)(H - H * tf[ i * 4 + ch] / 255.0));
	}
}


static void tf_set(
	int ch, 
	CPoint pt0, 
	CPoint pt1, 
	EVec4i rect,
	OglImage1D<CH_RGBA> &tf
	)
{
	EVec2d p0((pt0.x - rect[0]) / (double)rect[2], (pt0.y - rect[1]) / (double)rect[3]);
	EVec2d p1((pt1.x - rect[0]) / (double)rect[2], (pt1.y - rect[1]) / (double)rect[3]);


	if (p0[0] > p1[0]) p1.swap(p0);
	t_crop(0, 1, p0[0]);
	t_crop(0, 1, p0[1]);
	t_crop(0, 1, p1[0]);
	t_crop(0, 1, p1[1]);
	p0[1] = 1 - p0[1];
	p1[1] = 1 - p1[1];

	int idx0 = (int)(p0[0] * (TRANS_FUNC_SIZE - 1));
	int idx1 = (int)(p1[0] * (TRANS_FUNC_SIZE - 1));

	for (int i = idx0; i <= idx1; ++i)
	{
		double y = (p1[1] - p0[1]) / (idx1 - idx0 + 1) * (i - idx0) + p0[1];
		tf[i * 4 + ch] = (byte)(255 * y);
	}
	tf.setUpdated();
}












static CPen penR, penG, penB;


void DlgVisParam::OnPaint()
{
	CPaintDC dc(this); 

	static bool isFirst = true;
	if (isFirst) 
	{
		isFirst = false;
		penR.CreatePen(PS_SOLID, 2, RGB(0xFF, 0x00, 0x00));
		penG.CreatePen(PS_SOLID, 5, RGB(0x00, 0xFF, 0x00));
		penB.CreatePen(PS_SOLID, 2, RGB(0x00, 0x00, 0xFF));
	}

	//Prepare double buffering bitmap (todo!! 毎回作る必要ない)
	BITMAPINFO binfo;
	ZeroMemory(&binfo, sizeof(binfo));
	binfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biBitCount = 32;
	binfo.bmiHeader.biPlanes   = 1;
	binfo.bmiHeader.biWidth    = m_pcRect1[2];
	binfo.bmiHeader.biHeight   = m_pcRect1[3]; 

	byte    *bits1;
	HBITMAP  hbmp1 = CreateDIBSection(NULL, &binfo, DIB_RGB_COLORS, (void **)(&bits1), NULL, 0);
	CBitmap *cbmp1 = CBitmap::FromHandle( hbmp1 );

	binfo.bmiHeader.biWidth  = m_pcRect2[2];
	binfo.bmiHeader.biHeight = m_pcRect2[3];
	byte    *bits2;
	HBITMAP  hbmp2 = CreateDIBSection(NULL, &binfo, DIB_RGB_COLORS, (void **)(&bits2), NULL, 0);
	CBitmap *cbmp2 = CBitmap::FromHandle(hbmp2);

	CDC bmpDC1, bmpDC2;
	bmpDC1.CreateCompatibleDC(&dc);
	bmpDC2.CreateCompatibleDC(&dc);
	CPen    *oldPen1 = bmpDC1.SelectObject(&penG);
	CPen    *oldPen2 = bmpDC2.SelectObject(&penG);
	CBitmap *oldBmp1 = bmpDC1.SelectObject(cbmp1);
	CBitmap *oldBmp2 = bmpDC2.SelectObject(cbmp2);


	//draw background--------------------------------------
	memcpy( bits1, m_pcBgBits1, sizeof(byte) * m_pcRect1[2] * m_pcRect1[3] * 4);
	memcpy( bits2, m_pcBgBits2, sizeof(byte) * m_pcRect2[2] * m_pcRect2[3] * 4);

	////draw transfunc-------------------------------------
	tf_draw(0, m_pcRect1[2], m_pcRect1[3], ImageManager::getInst()->m_imgTf, &bmpDC1);
	tf_draw(1, m_pcRect2[2], m_pcRect2[3], ImageManager::getInst()->m_imgTf, &bmpDC2);

	//draw text -------------------------------------------
	CRect rc1, rc2;
	m_pc1.GetClientRect(&rc1);
	m_pc2.GetClientRect(&rc2);
	bmpDC1.SetBkMode(TRANSPARENT);
	bmpDC2.SetBkMode(TRANSPARENT);
	bmpDC1.SetTextColor(0x001515FF);
	bmpDC2.SetTextColor(0x001515FF);
	bmpDC1.DrawText(CString("transFunc1"), -1, &rc1, DT_SINGLELINE);
	bmpDC2.DrawText(CString("transFunc2"), -1, &rc2, DT_SINGLELINE);

	//swap buffer
	m_pc1.GetDC()->BitBlt(0, 0, m_pcRect1[2], m_pcRect1[3], &bmpDC1, 0, 0, SRCCOPY);
	m_pc2.GetDC()->BitBlt(0, 0, m_pcRect2[2], m_pcRect2[3], &bmpDC2, 0, 0, SRCCOPY);

	bmpDC1.SelectObject(oldPen1); 
	bmpDC1.SelectObject(oldBmp1);
	bmpDC2.SelectObject(oldPen2);  
	bmpDC2.SelectObject(oldBmp2);

	//TODO最初の一回だけでたぶんＯｋーーテストする。
	DeleteDC(bmpDC1);
	DeleteDC(bmpDC2);
}






















static bool isInRect(const EVec4i &rect, const CPoint p)
{
	return
		rect[0] <= p.x && p.x <= rect[0] + rect[2] &&
		rect[1] <= p.y && p.y <= rect[1] + rect[3];
}


void DlgVisParam::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (isInRect(m_pcRect1, point))
	{
		m_preP = point;
		m_drawTf = true;
		m_drawTfCh = 0;
		SetCapture();
	}

	if (isInRect(m_pcRect2, point))
	{
		m_preP     = point;
		m_drawTf   = true;
		m_drawTfCh = 1;
		SetCapture();
	}
}


void DlgVisParam::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_drawTf = false;
	ReleaseCapture();
	CSproutViewerView::getInst()->RedrawWindow();
}


void DlgVisParam::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_drawTf) return;
		
	EVec4i &rect = (m_drawTfCh == 0) ? m_pcRect1 : m_pcRect2;
	tf_set(m_drawTfCh, point, m_preP, rect, ImageManager::getInst()->m_imgTf);
	m_preP = point;

	Invalidate(FALSE);
	UpdateWindow();
	CSproutViewerView::getInst()->RedrawWindow();
}



void DlgVisParam::OnBnClickedCheckBkWhite()
{
	m_check_bgWhite.SetCheck(1);
	m_check_bgBlack.SetCheck(0);
	CSproutViewerView::getInst()->RedrawWindow();
}
void DlgVisParam::OnBnClickedCheckBkBlack()
{
	m_check_bgWhite.SetCheck(0);
	m_check_bgBlack.SetCheck(1);
	CSproutViewerView::getInst()->RedrawWindow();
}

void DlgVisParam::OnBnClickedCheckVolume()
{
	CSproutViewerView::getInst()->RedrawWindow();
}

void DlgVisParam::OnBnClickedCheckVisMask()
{
	CSproutViewerView::getInst()->RedrawWindow();
}

void DlgVisParam::OnBnClickedCheckPlaneXy(){ CSproutViewerView::getInst()->RedrawWindow(); }
void DlgVisParam::OnBnClickedCheckPlaneYz(){ CSproutViewerView::getInst()->RedrawWindow(); }
void DlgVisParam::OnBnClickedCheckPlaneZx(){ CSproutViewerView::getInst()->RedrawWindow(); }
void DlgVisParam::OnBnClickedCheckFrame  (){ CSproutViewerView::getInst()->RedrawWindow(); }
void DlgVisParam::OnBnClickedCheckVisSurf(){ CSproutViewerView::getInst()->RedrawWindow(); }



void DlgVisParam::OnBnClickedButtonComputeSurf()
{
	ImageManager::getInst()->updateSurfFromMask();
	CSproutViewerView::getInst()->RedrawWindow();
}


void DlgVisParam::OnBnClickedButtonComputeMask()
{
	ImageManager::getInst()->updateMask();
	CSproutViewerView::getInst()->RedrawWindow();
}
