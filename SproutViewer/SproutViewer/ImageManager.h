/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



#pragma once



#include <vector>
using namespace std;

#include "COMMON\OglImage.h"
#include "DlgVisParam.h"

#include "COMMON\tmesh.h"
#include "COMMON\tmarchingcubes.h"

#define HIST_BIN_SIZE   256
#define TRANS_FUNC_SIZE 256




class TVolumeInt16
{
public:
    int    W , H, D;
    double px,py,pz;
    short* img;
    TVolumeInt16(){
        W = H = D = 0;
        img = 0;
    }

    TVolumeInt16(int _W, int _H, int _D){
        img = 0;
        allocate(_W,_H,_D);
    }

    TVolumeInt16( const TVolumeInt16 &src){
        img = 0;
        set( src );
    }
    TVolumeInt16& operator=(const TVolumeInt16& src){
        set(src);
        return *this;
    }

    void set( const TVolumeInt16 &src)
    {
        if( img != 0) delete[] img;
        W = src.W; H = src.H; D = src.D; img = 0;
        if( src.img != 0 ) {
            img = new short[ W * H * D ];
            memcpy(img, src.img, sizeof( short) * W*H*D );
        }
    }

    ~TVolumeInt16(){
        if( img != 0 ) delete[] img;
        img = 0;
    }
    void allocate( int _W, int _H, int _D)
    {
        if( img != 0 ) delete[] img;
        W = _W;
        H = _H;
        D = _D;
        img = new short[W*H*D];
        memset( img, 0, sizeof( short ) * W*H*D );
    }

    void flipInZ()
    {
        short *tmp = new short[W*H];
        for( int i=0; i < D/2; ++i)
        {
            short *img1 = &(img[   i     * W * H]);
            short *img2 = &(img[ (D-1-i) * W * H]);
            memcpy(  tmp, img1, sizeof( short ) * W * H);
            memcpy( img1, img2, sizeof( short ) * W * H);
            memcpy( img2, tmp , sizeof( short ) * W * H);
        }
        delete[] tmp;
    }
};









class ImageManager
{
	ImageManager();
public:
	~ImageManager();


	static ImageManager* getInst(){
		static ImageManager p;
		return &p;
	}

	//vis param dlg
	DlgVisParam m_dlg;

	//4DCT image & 4DCT mask
	vector< TVolumeInt16* > m_img4D ;
	vector< byte*         > m_mask4D;
	vector< TMesh         > m_surf4D;

	//a volume for sending GPU
	EVec2i      m_winLv  ;
	OglImage3D  m_vol    ;
	OglImage3D  m_volMask;

	//Transfer functions 
	OglImage1D<CH_RGBA> m_imgTf    ;
	OglImage1D<CH_RGBA> m_imgPsu   ; // func: intensity --> psude color 
	OglImage1D<CH_RGBA> m_imgMskCol; // func: maskID    --> color

	
	float m_histVol [ HIST_BIN_SIZE ];
	float m_histGmag[ HIST_BIN_SIZE ];




	void load4DCT(CString topDir, int flg_DCMs_or_traw );
	void loadMaskAtInitFrame(CString fname);

	void UpdateWindowLevel(float minV, float maxV);
	void updateVisVolume( int winLvMin, int winLvMax, int time );
	void updateHistogram();
	
	void updateMask();
	void updateSurfFromMask();

	EVec3i getReso   (){return EVec3i( m_img4D[0]->W , m_img4D[0]->H , m_img4D[0]->D  ); }
	EVec3f getPitch  (){return EVec3f( (float)m_img4D[0]->px, (float)m_img4D[0]->py, (float)m_img4D[0]->pz ); }
	EVec3f getCuboidF(){return EVec3f( (float)m_img4D[0]->px * m_img4D[0]->W, 
		                               (float)m_img4D[0]->py * m_img4D[0]->H,
		                               (float)m_img4D[0]->pz * m_img4D[0]->D);}


private:
	void fitRotation();
	void saveMask(const int W, const int H, const int D, const byte *img, const int maskN, const string  fname);

};

