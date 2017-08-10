/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



#include "stdafx.h"
#include "TCore.h"
#include "ImageManager.h"
#include "COMMON\CrsSecCore.h"
#include "SproutViewerView.h"


TCore::TCore() : 
	m_shaderVol    ("shader/volVtx.glsl", "shader/volFlg.glsl"    ), m_shaderCrs    ("shader/crssecVtx.glsl", "shader/crssecFlg.glsl"    ),
	m_shaderVolMask("shader/volVtx.glsl", "shader/volFlg_Msk.glsl"), m_shaderCrsMask("shader/crssecVtx.glsl", "shader/crssecFlg_Msk.glsl")
{
	m_bL = m_bR = m_bM =  false;
}


TCore::~TCore()
{
}


void TCore::KeyDown   ( UINT nChar, UINT nRepCnt, UINT nFlags, OglForMFC &ogl){
	if (nChar == VK_RIGHT) {
		ogl.KeyDown_RightRot();
	} else if (nChar == VK_LEFT) {
		ogl.KeyDown_LeftRot();
	} else if (nChar == VK_UP) {
		ogl.KeyDown_UpRot();
	} else if (nChar == VK_DOWN) {
		ogl.KeyDown_DownRot();
	}
	ogl.Redraw();
}
void TCore::KeyUp     ( UINT nChar, UINT nRepCnt, UINT nFlags, OglForMFC &ogl){
}


void TCore::LBtnDown  ( CPoint p, OglForMFC &ogl )
{
	m_bL = true;
	ogl.BtnDown_Trans(p);
}
void TCore::LBtnUp    ( CPoint p, OglForMFC &ogl )
{
	m_bL = false;
	ogl.BtnUp();
}

void TCore::RBtnDown  ( CPoint p, OglForMFC &ogl )
{
	m_bR = true;
	ogl.BtnDown_Rot(p);
}

void TCore::RBtnUp    ( CPoint p, OglForMFC &ogl )
{
	m_bR = false;
	ogl.BtnUp();
}

void TCore::MBtnDown  ( CPoint p, OglForMFC &ogl )
{
	m_bM = true;
	ogl.BtnDown_Zoom(p);
}

void TCore::MBtnUp    ( CPoint p, OglForMFC &ogl )
{
	m_bM = false;
	ogl.BtnUp();

}

void TCore::MouseWheel(const CPoint &p, int nFlags, short zDelta, OglForMFC &ogl) 
{
	fprintf( stderr, "w:%d\n", zDelta);

	EVec3f cuboid   = ImageManager::getInst()->getCuboidF();
	EVec3f pitch    = ImageManager::getInst()->getPitch  ();
	EVec3i reso     = ImageManager::getInst()->getReso   ();
	const bool bXY  = ImageManager::getInst()->m_dlg.m_check_planeXY.GetCheck()?true:false;
	const bool bYZ  = ImageManager::getInst()->m_dlg.m_check_planeYZ.GetCheck()?true:false;
	const bool bZX  = ImageManager::getInst()->m_dlg.m_check_planeZX.GetCheck()?true:false;

	EVec3f eyeRay, eyePos;
	ogl.GetCursorRay(p, eyePos, eyeRay);

	CRSSEC_ID id    = CrsSecCore::getInst()->pickCrosSec(bXY, bYZ, bZX, cuboid, eyePos, eyeRay);
	if( id != CRSSEC_NON ) CrsSecCore::getInst()->moveCrossSec(reso, pitch, id, zDelta);
	else ogl.ZoomCam(zDelta * 0.1f);

	ogl.Redraw();
}


void TCore::LBtnDblCkl( CPoint p, OglForMFC &ogl ){}
void TCore::RBtnDblCkl( CPoint p, OglForMFC &ogl ){}
void TCore::MBtnDblCkl( CPoint p, OglForMFC &ogl ){}


void TCore::MouseMove ( CPoint p, OglForMFC &ogl )
{
	if( !m_bL && !m_bM && !m_bR) return;

	ogl.MouseMove(p);
	ogl.Redraw();
}







void TCore::drawScene(const EVec3f &camP, const EVec3f &camF, const EVec3f &camY)
{
	const EVec3f cuboid   = ImageManager::getInst()->getCuboidF();
	const EVec3i reso     = ImageManager::getInst()->getReso ();
	const EVec3f pitch    = ImageManager::getInst()->getPitch();
	
	const bool  bGradMag = false;
	const bool  bPsuedo  = true ;
	const bool  bXY      = ImageManager::getInst()->m_dlg.m_check_planeXY .GetCheck()?true:false;
	const bool  bYZ      = ImageManager::getInst()->m_dlg.m_check_planeYZ .GetCheck()?true:false;
	const bool  bZX      = ImageManager::getInst()->m_dlg.m_check_planeZX .GetCheck()?true:false;
	const bool  bDrawVol = ImageManager::getInst()->m_dlg.m_check_volume  .GetCheck()?true:false;
	const bool  bDrawMsk = ImageManager::getInst()->m_dlg.m_check_Mask    .GetCheck()?true:false;
	const bool  bDrawFrm = ImageManager::getInst()->m_dlg.m_check_frame   .GetCheck()?true:false;
	const bool  bDrawSrf = ImageManager::getInst()->m_dlg.m_check_surf    .GetCheck()?true:false;
	const float alpha    = ImageManager::getInst()->m_dlg.getTransAlpha();


	if( bDrawFrm )
	{
		glDisable( GL_LIGHTING );
		glLineWidth( 5 );
		glBegin( GL_LINES );
			glColor3d(1,0,0); glVertex3d(0,0,0); glVertex3d(10,0,0);
			glColor3d(0,1,0); glVertex3d(0,0,0); glVertex3d(0,10,0);
			glColor3d(0,0,1); glVertex3d(0,0,0); glVertex3d(0,0,10);
		glEnd();

		t_drawFrame(cuboid);
	}


	const int sliceN   = (int)( ((m_bL || m_bR || m_bM) ? 0.5: 1.0 ) * ImageManager::getInst()->m_dlg.m_slider_sliceN.GetPos() );


	//bind volumes ---------------------------------------
	glActiveTextureARB(GL_TEXTURE0);
	ImageManager::getInst()->m_vol.bindOgl();
	
	//glActiveTextureARB(GL_TEXTURE1);
	//ImageManager::getInst()->m_volGmag.bindOgl();
	//glActiveTextureARB(GL_TEXTURE2);
	//ImageManager::getInst()->m_volFlg.bindOgl(false);
	glActiveTextureARB(GL_TEXTURE3);
	ImageManager::getInst()->m_volMask.bindOgl(false);
	glActiveTextureARB(GL_TEXTURE4);
	ImageManager::getInst()->m_imgTf.bindOgl(false);
	glActiveTextureARB(GL_TEXTURE5);
	ImageManager::getInst()->m_imgPsu.bindOgl();
	glActiveTextureARB(GL_TEXTURE6);
	ImageManager::getInst()->m_imgMskCol.bindOgl(false);

	/*
	if (m_bDrawStr)
	{
		const int N = (int) m_stroke.size();
		int *idx = new int[N];
		for( int i=0; i < N; ++i) idx[i] = i;

		glColor3d( 1,1,0);
		glLineWidth( 3 );

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_stroke.data() );
		glDrawElements (GL_LINE_STRIP , (int)m_stroke.size(), GL_UNSIGNED_INT, idx);
		glDisableClientState(GL_VERTEX_ARRAY);
	
		delete[] idx;
	}
	*/

	glColor3d(1, 1, 1);

	if (bDrawMsk)
	{
		m_shaderCrsMask.bind(0, 1, 2, 3, 6, reso, bGradMag, true);
		CrsSecCore::getInst()->drawCrosSec( bXY, bYZ, bZX, cuboid);
		m_shaderCrsMask.unbind();
		
		if ( bDrawVol )
		{
			glDisable( GL_DEPTH_TEST);
			glEnable ( GL_BLEND);
			m_shaderVolMask.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, false);
			t_drawSlices(sliceN, camP, camF, cuboid );
			m_shaderVolMask.unbind();
			glDisable( GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}
	}
	else
	{
		m_shaderCrs.bind(0, 1, 2, 3, 6, reso, bGradMag, false);
		CrsSecCore::getInst()->drawCrosSec( bXY, bYZ, bZX, cuboid);
		m_shaderCrs.unbind();

		if ( bDrawVol )
		{
			glDisable( GL_DEPTH_TEST);
			glEnable ( GL_BLEND);
			m_shaderVol.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, false);
			t_drawSlices(sliceN, camP, camF, cuboid );
			m_shaderVol.unbind();
			glDisable( GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}
	}	


	if (bDrawSrf)
	{
		const int time= ImageManager::getInst()->m_dlg.m_slider_timeI.GetPos();
		ImageManager::getInst()->m_surf4D[time].draw();
		
	}


}