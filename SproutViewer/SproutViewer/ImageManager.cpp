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
		fprintf( stderr,"%s  -- %d %d\n" , path, i, (int)fNames[i].size());
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
                fprintf(stderr, "error = %s \n", fNames[idx][i] );
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



void ImageManager::fitRotation() {

	const int N = m_img4D.size();

	int Width = m_img4D[0]->W;
	int Height = m_img4D[0]->H;
	int Depth = m_img4D[0]->D;

	double center[2] = { (Width - 1)*0.5 , (Height - 1)*0.5 };

	const int slice = 100;
	const double convert = 3.141592653589793 / 180;
	const double degreeRange = 20;

	double x, y, xx;
	double rad, min, minRad;
	unsigned int sum;
	int dif;

	double turn_mat[2][2] = { 0 };


	fprintf(stderr, "Turn start\n");
	for (int frame = 1; frame < N; frame++) {

		fprintf(stderr, "frame[%d]: start ", frame);
		
		for (double degree = -degreeRange / 2; degree <= degreeRange / 2; degree += 0.01) {
			rad = degree * convert;
			sum = 0;				
			for (int h = 0; h < Height; h++)
				for (int w = 0; w < Width; w++) {

					turn_mat[0][0] =   turn_mat[1][1] = cos(rad);
					turn_mat[1][0] = -(turn_mat[0][1] = sin(rad));
					
					x = w - center[0];
					y = h - center[1];

					xx = turn_mat[0][0] * x + turn_mat[0][1] * y + center[0];
					y  = turn_mat[1][0] * x + turn_mat[1][1] * y + center[0];

					if (xx >= 0 && xx < Width && y >= 0 && y < Height) {
						dif = m_img4D[0]->img[w + h*Width + slice*Width*Height] - m_img4D[frame]->img[(int)(xx)+(int)(y)*Width + slice*Width*Height];
						sum += sqrt(pow(dif, 2));
					}
				}
			if (degree == -degreeRange / 2 || sum < min) {
				min = sum;
				minRad = rad;
			}
		}
		
		fprintf(stderr, "-> angle determined ");




		short* turn_img = new short[Width*Height*Depth];
		short imgs[4] = { 0 };

		memcpy(turn_img, m_img4D[frame]->img, sizeof(short)*Width*Height*Depth);

		turn_mat[0][0] = turn_mat[1][1] = cos(minRad);
		turn_mat[1][0] = -(turn_mat[0][1] = sin(minRad));

		for (int d = 0; d < Depth; d++)
			for (int h = 0; h < Height; h++)
				for (int w = 0; w < Width; w++) {
					x = w - center[0];
					y = h - center[1];

					xx = turn_mat[0][0] * x + turn_mat[0][1] * y + center[0];
					y = turn_mat[1][0] * x + turn_mat[1][1] * y + center[1];

					if (xx >= 0 && xx < Width && y >= 0 && y < Height) {

						if (xx < Width - 1 && y < Height - 1) {
							imgs[0] = m_img4D[frame]->img[(int)xx +     (int)y      *Width + d*Width*Height];
							imgs[1] = m_img4D[frame]->img[(int)xx + 1 + (int)y      *Width + d*Width*Height];
							imgs[2] = m_img4D[frame]->img[(int)xx +     (int)(y + 1)*Width + d*Width*Height];
							imgs[3] = m_img4D[frame]->img[(int)xx + 1 + (int)(y + 1)*Width + d*Width*Height];

							turn_img[w + h*Width + d*Width*Height] =
								(1 - (xx - (int)xx))*(1 - (y - (int)y))*imgs[0] +
								(xx - (int)xx)      *(1 - (y - (int)y))*imgs[1] +
								(1 - (xx - (int)xx))*(y - (int)y)	   *imgs[2] +
								(xx - (int)xx)      *(y - (int)y)	   *imgs[3];

						}
						else {
							turn_img[w + h*Width + d*Width*Height] = m_img4D[frame]->img[(int)(xx)+(int)(y)*Width + d*Width*Height];
						}
					}
				}

		memcpy(m_img4D[frame]->img, turn_img, sizeof(short)*Width*Height*Depth);
		fprintf(stderr, "-> finish\n");

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


