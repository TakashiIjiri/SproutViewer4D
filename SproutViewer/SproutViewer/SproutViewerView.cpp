/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



// SproutViewerView.cpp : CSproutViewerView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "SproutViewer.h"
#endif

#include "SproutViewerDoc.h"
#include "SproutViewerView.h"
#include "TCore.h"
#include "ImageManager.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSproutViewerView

IMPLEMENT_DYNCREATE(CSproutViewerView, CView)

BEGIN_MESSAGE_MAP(CSproutViewerView, CView)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT        , &CView::OnFilePrint       )
	ON_COMMAND(ID_FILE_PRINT_DIRECT , &CView::OnFilePrint       )
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE   ()
	ON_WM_DESTROY  ()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN  ()
	ON_WM_KEYUP    ()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN  ()
	ON_WM_LBUTTONUP    ()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN  ()
	ON_WM_MBUTTONUP    ()
	ON_WM_SIZE         ()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN  ()
	ON_WM_RBUTTONUP    ()
	ON_WM_ERASEBKGND   ()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CSproutViewerView コンストラクション/デストラクション

CSproutViewerView *CSproutViewerView::m_myself = 0;

CSproutViewerView::CSproutViewerView()
{
	m_myself = this;
}

CSproutViewerView::~CSproutViewerView()
{
}

BOOL CSproutViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CSproutViewerView 描画


// CSproutViewerView 印刷

BOOL CSproutViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 既定の印刷準備
	return DoPreparePrinting(pInfo);
}

void CSproutViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CSproutViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}


// CSproutViewerView 診断

#ifdef _DEBUG
void CSproutViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CSproutViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSproutViewerDoc* CSproutViewerView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSproutViewerDoc)));
	return (CSproutViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSproutViewerView メッセージ ハンドラー
int CSproutViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//added by takashi
	m_ogl.OnCreate( this );
	
	return 0;
}

void CSproutViewerView::OnDestroy()
{
	CView::OnDestroy();

	//added by takashi
	m_ogl.OnDestroy( );
}


void CSproutViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	m_ogl.OnSize( cx, cy );
}

BOOL CSproutViewerView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}


void CSproutViewerView::OnDraw(CDC* /*pDC*/)
{
	CSproutViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(ImageManager::getInst()->m_dlg.m_check_bgBlack.GetCheck() )
		m_ogl.SetBgColor(EVec4f(0,0,0,0));
	else 
		m_ogl.SetBgColor(EVec4f(1,1,1,1));


	m_ogl.OnDrawBegin();

	EVec3f camP = m_ogl.GetCamPos();
	EVec3f camF = m_ogl.GetCamCnt();
	EVec3f camY = m_ogl.GetCamUp ();
	TCore::getInst()->drawScene(camP, camF, camY);

	m_ogl.OnDrawEnd();
}










void CSproutViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	TCore::getInst()->MouseMove(point, m_ogl);
}


void CSproutViewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TCore::getInst()->KeyDown(nChar, nRepCnt, nFlags, m_ogl);
}


void CSproutViewerView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TCore::getInst()->KeyUp( nChar, nRepCnt, nFlags, m_ogl);
}

void CSproutViewerView::OnLButtonDblClk(UINT nFlags, CPoint point){ TCore::getInst()->LBtnDblCkl( point , m_ogl); }
void CSproutViewerView::OnMButtonDblClk(UINT nFlags, CPoint point){ TCore::getInst()->MBtnDblCkl( point , m_ogl); }
void CSproutViewerView::OnRButtonDblClk(UINT nFlags, CPoint point){ TCore::getInst()->RBtnDblCkl( point , m_ogl); }
void CSproutViewerView::OnLButtonDown  (UINT nFlags, CPoint point){ TCore::getInst()->LBtnDown  ( point , m_ogl); }
void CSproutViewerView::OnMButtonDown  (UINT nFlags, CPoint point){ TCore::getInst()->MBtnDown  ( point , m_ogl); }
void CSproutViewerView::OnRButtonDown  (UINT nFlags, CPoint point){ TCore::getInst()->RBtnDown  ( point , m_ogl); }
void CSproutViewerView::OnLButtonUp    (UINT nFlags, CPoint point){ TCore::getInst()->LBtnUp    ( point , m_ogl); }
void CSproutViewerView::OnMButtonUp    (UINT nFlags, CPoint point){ TCore::getInst()->MBtnUp    ( point , m_ogl); }
void CSproutViewerView::OnRButtonUp    (UINT nFlags, CPoint point){ TCore::getInst()->RBtnUp    ( point , m_ogl); }







//
//http://www.kab-studio.biz/Programing/Codian/ShellExtension/02.html
CString CSproutViewerView::BrowseForFolder( HWND p_hWnd, CString p_cSetStr, CString p_cRootStr, CString p_cCaptionStr, UINT p_uiFlags )
{
	BOOL		 bRes;
	char		 chPutFolder[MAX_PATH];
	LPITEMIDLIST pidlRetFolder;
	BROWSEINFO	  stBInfo;
	CString		cRetStr;

	//　構造体を初期化します。
	stBInfo.pidlRoot       = NULL;	//ルートフォルダです。
	stBInfo.hwndOwner      = p_hWnd;	//表示するダイアログの親ウィンドウのハンドルです。
	stBInfo.pszDisplayName = chPutFolder;	//選択されているフォルダ名を受けます。
	stBInfo.lpszTitle      = p_cCaptionStr;	//説明の文字列です。
	stBInfo.ulFlags        = p_uiFlags;	//表示フラグです。
	stBInfo.lpfn           = NULL;	//ダイアログプロシージャへのポインタです。
	stBInfo.lParam         = 0L;	//プロシージャに送るパラメーターです。

	//　ダイアログボックスを表示します。
	pidlRetFolder = ::SHBrowseForFolder( &stBInfo );

	//　pidlRetFolderにはそのフォルダを表すアイテムＩＤリストへのポインタが
	//　入っています。chPutFolderには選択されたフォルダ名（非フルパス）だけ
	//　しか入っていないので、このポインタを利用します。

	if( pidlRetFolder == NULL )
	{
		AfxMessageBox("ディレクトリを指定してください");
		exit(0);
	}

	//　フルパスを取得します。
	bRes = ::SHGetPathFromIDList( pidlRetFolder, chPutFolder );
	if( bRes != FALSE ) cRetStr = chPutFolder;

	::CoTaskMemFree( pidlRetFolder );

	return cRetStr;
}


void CSproutViewerView::load4dct(int flg_DCM_traw)
{
	m_topDir = BrowseForFolder(NULL, "select folder", "", "select folder1", 1);
	ImageManager::getInst()->load4DCT( m_topDir,flg_DCM_traw );
	//postInitialization();
}

void CSproutViewerView::loadMask()
{
	m_topDir = BrowseForFolder(NULL, "select folder", "", "select folder1", 1);
	ImageManager::getInst()->loadMask(m_topDir);
	//postInitialization();
}

BOOL CSproutViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CPoint point = pt;
	CRect rect;
	this->GetWindowRect(&rect);
	point.x -= rect.left;
	point.y -= rect.top;
	TCore::getInst()->MouseWheel(point, nFlags, zDelta, m_ogl);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
