/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



#include "stdafx.h"
#include "ImageManager.h"
#include "SproutViewerView.h"
#include "DlgLoadFrameIdx.h"
#include "./3rdParty/dcmtk/tdcmtk.h"
#include "resource.h"


ImageManager::ImageManager()
{
	m_dlg.Create( IDD_DIALOG_VISPARAM );
	m_dlg.ShowWindow( SW_SHOW );

	fprintf( stderr, "constructure ImageManager-----\n" );
	load4DCT("");
	fprintf( stderr, "constructure ImageManager-----DONE\n" );

}


ImageManager::~ImageManager()
{
}



////////////////////////////////////////////////////////////////////////////////////
// 4d ct loader ////////////////////////////////////////////////////////////////////

void t_get4DFilePaths( CString topDirPath, vector< vector<CString> > &fNames )
{

	//get folders' paths
	//http://www.dinop.com/vc/allfile_in_folder.html
	vector<CString> folders;
	{
		if( topDirPath.Right(1) != "\\") topDirPath += "\\";
		topDirPath += "*.*";

		CFileFind cFind;
		BOOL bContinue = cFind.FindFile( topDirPath );

		while(bContinue)
		{
			bContinue = cFind.FindNextFile();
			if( cFind.IsDirectory() && !cFind.IsDots() ) folders.push_back( cFind.GetFilePath() );
		}
	}
	
	fNames.resize( folders.size() );
	for( int i=0; i < folders.size(); ++i)
	{
		CString path = folders[i];
		if( path.Right(1) != "\\") path += "\\";
		path += "*.DCM";

		CFileFind cFind;
		BOOL bContinue = cFind.FindFile( path );

		while(bContinue)
		{
			bContinue = cFind.FindNextFile();
			fNames[i].push_back( cFind.GetFilePath() );
		}
		fprintf( stderr,"%s  -- %d %d\n" , path.GetString(), i, (int)fNames[i].size());
	}

}




bool t_open4DImg( const vector< vector<CString> > &fNames, vector< TVolumeInt16* > &img4D )
{

    const int frameN = (int)fNames.size();
	int startI, endI;

	DlgLoadFrameIdx dlg;
	if( dlg.myDoModal( frameN, startI, endI ) != IDOK || startI >= endI ) exit(0);


    for( int idx = startI; idx < endI; ++idx )
    {
        fprintf(stderr, "load %d/%d   %d...", idx, frameN, (int)fNames[idx].size() );

        vector<EVec3d> frame_pitchXY_zPos; // {(x,y,zSlizePos)}

        for( int i=0; i < fNames[idx].size(); ++i)
        {
			//load dicoms 
            Tdcmtk tdcmtk( fNames[idx][i] );
            int W, H, fNum;
            tdcmtk.getSize( W, H, fNum );
			//int chNum, bitNum, bSign;
            //tdcmtk.getFormat( chNum, bitNum, bSign);


            if( i == 0 ) img4D.push_back( new TVolumeInt16(W, H, (int) fNames[idx].size()) ); //initialize

            if( img4D.back()->W != W || img4D.back()->H != H  ){
                fprintf(stderr, "inconsistent error\n");
                return false;
            }

            frame_pitchXY_zPos.push_back( EVec3d( tdcmtk.getPitchX(), tdcmtk.getPitchY(), tdcmtk.getZPos() ));


            if( !tdcmtk.getPixelsAs<short>( &img4D.back()->img[ i*W*H ] ) )
			{
                fprintf(stderr, "error = %s \n", fNames[idx][i].GetString() );
            }
        }

        img4D.back()->px = ( frame_pitchXY_zPos[0][0] == -1.0) ? 1 : frame_pitchXY_zPos[0][0];
        img4D.back()->py = ( frame_pitchXY_zPos[0][1] == -1.0) ? 1 : frame_pitchXY_zPos[0][1];
        img4D.back()->pz = fabs( frame_pitchXY_zPos[0][2] - frame_pitchXY_zPos[1][2]);

        fprintf(stderr,"to Flipping volumes in stack direction! ");
        img4D.back()->flipInZ();
        fprintf(stderr,"Done\n");
    }
    return true;
}










static bool t_LoadDefoultVolume
	(
	EVec3i &reso,
	EVec3f &pitch,
	float   cx,
	TVolumeInt16* &img
	)
{
	const int N = 64, R = N/5;
	reso  << N, N, N;
	pitch << 1, 1, 1;
	img = new TVolumeInt16(reso[0], reso[1], reso[2]);
	img->px = pitch[0];
	img->py = pitch[1];
	img->pz = pitch[2];

	EVec3f c1(cx + 2*R, 2*R, 2*R), c2(cx + 3*R, 3 * R, 3 * R);

	for (int z = 0; z < reso[2]; ++z)
	{
		for (int y = 0; y < reso[1]; ++y)
		{
			for (int x = 0; x < reso[0]; ++x)
			{
				EVec3f p((float)x, (float)y, (float)z);
				float d1 = (p - c1).norm();
				float d2 = (p - c2).norm();
				float v = 3000 * exp(-(d1-R)*(d1-R) / 10 ) + 3000 * exp(-(d2-R)*(d2-R) / 10);
				img->img[x + y * reso[0] + z * reso[1] * reso[0]] = (short)(v - 1500.0f);
			}
		}
	}
	return true;
}














void ImageManager::load4DCT(CString topDirPath)
{

	//clear 4d ct volume 
	for( int i=0; i < m_img4D.size(); ++i) delete m_img4D[i];
	m_img4D.clear();


	//load 4d ct
	if( topDirPath.GetLength() == 0)
	{
		for( int i=0; i < 30; ++i)
		{
			TVolumeInt16 *img;
			t_LoadDefoultVolume( EVec3i(64,64,64), EVec3f(0.05f,0.05f,0.05f ), i*5.0f, img);
			m_img4D.push_back( img );
		}
	}
	else
	{
		vector< vector<CString> > fNames;
		t_get4DFilePaths( topDirPath, fNames );
		t_open4DImg( fNames, m_img4D    );
		fitRotation();
	}
	

	if( m_img4D.size() == 0 || m_img4D[0]->W == 0 || m_img4D[0]->H == 0 || m_img4D[0]->D == 0 ) exit(0);


	//check resolution 
    const int W = m_img4D[0]->W;
    const int H = m_img4D[0]->H;
    const int D = m_img4D[0]->D;

    for( int i=0; i < m_img4D.size(); )
    {
        if( W != m_img4D[i]->W || H != m_img4D[i]->H || D != m_img4D[i]->D  ){
            fprintf(stderr, "wrong size volume exist!!!! %d \n", i);
            m_img4D.erase( m_img4D.begin() + i);
        }
        else ++i;
    }

    //get min and max (for Window level range)
    m_winLv[0] = SHRT_MAX;
    m_winLv[1] = SHRT_MIN;

    for(int  i = 0; i < W*H*D; ++i)
	{
        m_winLv[0] = min( m_winLv[0], (short) m_img4D[0]->img[i] );
        m_winLv[1] = max( m_winLv[1], (short) m_img4D[0]->img[i] );
    }

    fprintf(stderr, "Window Level is (%d %d) \n" , m_winLv[0], m_winLv[1]);


	//allocate other fields 
    m_vol.Allocate( W, H, D );
    m_vol.SetValue( m_img4D[0]->img, (short) m_winLv[0], (short) m_winLv[1]);


	m_imgPsu   .AllocateHeuImg(256);
	m_imgMskCol.Allocate      (256);
    m_imgTf.Allocate(TRANS_FUNC_SIZE);
	for (int i = 0; i < TRANS_FUNC_SIZE; ++i) m_imgTf[4 * i] = m_imgTf[4 * i + 1] = i;

	//compute gradient(yet) and histogram 
	//updateGradVolume();
	updateHistogram();

	//update dialog
	m_dlg.New4DCTLoaded ( (int) m_img4D.size(), getReso(), getPitch(), m_winLv);
	m_dlg.HistogramUpdated(HIST_BIN_SIZE, m_histVol, m_histGmag);
}








//      0     1     2     3     4
//   |-----|-----|-----|-----|-----|     N=5, pitch = 0.1
//   0    0.1   0.2   0.3   0.4   0.5


// !!!!!!!!!note!!!!!!!!!!!!!! 
// pix index (int) --> 2d space (double) 
// (u,v) --> ( (u+0.5)* pitchX, (v+0.5)*pitchY )

// 2D space (double) --> pix index
// (x,y) --> ((int)( x / pitchX ), (int)( y / pitchY ))

static double t_findRotAngle(
	const int    W , // resolution 
	const int    H , // resolution 
	const double px, // pitch in W dir
	const double py, // pitch in H dir
	const double thetaPiv  , //consider theta in [piv-range, piv+range]
	const double thetaRange,
	const double thetaStep , 
	const short  *img1,
	const short  *img2
	)
{
	const EVec2d pixCenter( (double)W / 2.0 * px, (double)H / 2.0 * py) ; 

	double foundTheta = 0;
	double minDeg = DBL_MAX;

	for (double t = thetaPiv - thetaRange; t <= thetaPiv + thetaRange; t += thetaStep) 
	{
		double theta = t * M_PI / 180.0;
		EMat2d M;
		M <<  cos(theta), -sin(theta), 
			  sin(theta),  cos(theta);

		double sum = 0;
		for (int y = 100; y < H - 100; ++y)
		{
			for (int x = 100; x < W - 100; ++x) 
			{
				EVec2d p( (x+0.5) * px, (y+0.5)*py  ) ; 
				EVec2d pixPos = M*(p - pixCenter) + pixCenter;

				int xx = (int)( pixPos[0] / px );
				int yy = (int)( pixPos[1] / py );

				if (0 <= xx && xx < W && 0 <= yy && yy < H ) 
				{
					sum += (img1[x + y * W] - img2[ xx + yy * W ]) * (img1[x + y * W] - img2[ xx + yy * W ]);
				}
			}
		}

		if (sum < minDeg) 
		{
			minDeg = sum;
			foundTheta = theta;
		}
	}	
	return foundTheta * 180 / M_PI;
}








// implemented by Tomofumi, modified by takashi (20170325)
void ImageManager::fitRotation() 
{

	fprintf( stderr, "fitRotation-----------------------\n");
	if( m_img4D.size() == 0 ) return;

	//画像の解像度とpitch(画素の大きさ)
	const int    W   = m_img4D[0]->W;
	const int    H   = m_img4D[0]->H;
	const int    D   = m_img4D[0]->D;
	const double px  = m_img4D[0]->px;
	const double py  = m_img4D[0]->py;
	const int SLICE_I = 100;

	//現在の回転角度，エラーが積み重なる可能性があるので、逐次更新する．
	double fitAngle = 0;


	for (int frame = 1; frame < m_img4D.size(); frame++) 
	{
		//search best fitting angle 
		double tmp1 = t_findRotAngle(W,H,px,py, fitAngle, 10 , 0.1  , &m_img4D[0]->img[SLICE_I *W*H], &m_img4D[frame]->img[SLICE_I *W*H] );
		fitAngle    = t_findRotAngle(W,H,px,py, tmp1    , 0.1, 0.001, &m_img4D[0]->img[SLICE_I *W*H], &m_img4D[frame]->img[SLICE_I *W*H] );

		// rotate image		
		const EVec2d center( (double)W / 2.0 * px, (double)H / 2.0 * py) ; 
		const double theta = fitAngle * M_PI / 180.0;
		EMat2d M;
		M << cos(theta), -sin(theta), 
			 sin(theta),  cos(theta);

		short* tmpImg = new short[W*H*D];
		memcpy(tmpImg, m_img4D[frame]->img, sizeof(short)*W*H*D);

		for (int z = 0; z < D; z++)
		for (int y = 0; y < H; y++)
		for (int x = 0; x < W; x++)
		{
			EVec2d p( (x+0.5)*px, (y+0.5)*py );
			EVec2d pos = M * (p - center) + center;

			const int xi = (int) (pos[0] / px);
			const int yi = (int) (pos[0] / px);

			if ( 0<= xi && xi < W && 0 <= yi  && yi < H) 
			{
				m_img4D[frame]->img[z*W*H + y*W + x] = tmpImg[z*W*H + yi*W + xi];

				//todo linear interpolation
				
							//imgs[0] = turn_img[(int)xx + (int)y      *Width + d*Width*Height];
							//imgs[1] = turn_img[(int)xx + 1 + (int)y      *Width + d*Width*Height];
							//imgs[2] = turn_img[(int)xx + (int)(y + 1)*Width + d*Width*Height];
							//imgs[3] = turn_img[(int)xx + 1 + (int)(y + 1)*Width + d*Width*Height];

							//m_img4D[frame]->img[w + h*Width + d*Width*Height] =
							//	(1 - (xx - (int)xx))*(1 - (y - (int)y))*imgs[0] +
							//	(xx - (int)xx) *(1 - (y - (int)y))*imgs[1] +
							//	(1 - (xx - (int)xx))*(y - (int)y) *imgs[2] +
							//	(xx - (int)xx) *(y - (int)y)	*imgs[3];
			}
		}

		delete[] tmpImg;
	}





}


void ImageManager::UpdateWindowLevel(float minV, float maxV)
{
	m_vol.SetValue( m_img4D[ m_dlg.m_slider_timeI.GetPos() ]->img, (short)minV, (short)maxV);
	updateHistogram();
	m_dlg.HistogramUpdated(HIST_BIN_SIZE, m_histVol, m_histGmag);
}


void ImageManager::updateVisVolume( int winLvMin, int winLvMax, int time )
{
    if( time < 0 || m_img4D.size() - 1 < time) return ;
    m_vol.SetValue( m_img4D[ time ]->img, (short)winLvMin, (short)winLvMax );
    m_vol.setUpdated();
}


void ImageManager::updateHistogram()
{
	EVec3i reso = getReso();
	const int N = reso[0] * reso[1] * reso[2];

	for (int i = 0; i < HIST_BIN_SIZE; ++i) m_histGmag[i] = 0;
	for (int i = 0; i < HIST_BIN_SIZE; ++i) m_histVol [i] = 0;
	
	for (int i = 0; i < N; ++i) 
	{
		//m_histGmag[ m_volGmag[i] ] += 1;
		m_histVol [ m_vol    [i] ] += 1;
	}

	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) 
	{
		m_histGmag[i] = (float) log( 1.0 + m_histGmag[i] );
	}

	float maxV = 0, maxG = 0;
	for (int i = 5; i < HIST_BIN_SIZE-5; ++i) maxV = max(maxV , m_histVol [i]);
	for (int i = 5; i < HIST_BIN_SIZE-5; ++i) maxG = max(maxG , m_histGmag[i]);
	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) m_histVol [i] /= maxV;
	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) m_histGmag[i] /= maxG;

}


