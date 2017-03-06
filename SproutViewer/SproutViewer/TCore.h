/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/


#pragma once

#include "COMMON/OglForMFC.h"
#include "COMMON/GlslShader.h"


class TCore
{
	GlslShaderVolume m_volumeShader;
	GlslShaderCrsSec m_crssecShader;

	bool m_bL, m_bR, m_bM;


private:
	TCore();

public:
	~TCore();


	static TCore* getInst()
	{
		static TCore m_p;
		return &m_p;
	}

	void KeyDown   ( UINT nChar, UINT nRepCnt, UINT nFlags, OglForMFC &ogl );
	void KeyUp     ( UINT nChar, UINT nRepCnt, UINT nFlags, OglForMFC &ogl );
	void LBtnDown  ( CPoint p, OglForMFC &ogl );
	void RBtnDown  ( CPoint p, OglForMFC &ogl );
	void MBtnDown  ( CPoint p, OglForMFC &ogl );
	void LBtnUp    ( CPoint p, OglForMFC &ogl );
	void RBtnUp    ( CPoint p, OglForMFC &ogl );
	void MBtnUp    ( CPoint p, OglForMFC &ogl );
	void LBtnDblCkl( CPoint p, OglForMFC &ogl );
	void RBtnDblCkl( CPoint p, OglForMFC &ogl );
	void MBtnDblCkl( CPoint p, OglForMFC &ogl );
	void MouseMove ( CPoint p, OglForMFC &ogl );
	void MouseWheel(const CPoint &p, int nFlags, short zDelta, OglForMFC &ogl);

	void drawScene (const EVec3f &camP, const EVec3f &camF, const EVec3f &camY);
};

