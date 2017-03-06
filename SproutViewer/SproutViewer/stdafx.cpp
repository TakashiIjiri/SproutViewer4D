/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



// stdafx.cpp : �W���C���N���[�h SproutViewer.pch �݂̂�
// �܂ރ\�[�X �t�@�C���́A�v���R���p�C���ς݃w�b�_�[�ɂȂ�܂��B
// stdafx.obj �ɂ̓v���R���p�C���ς݌^��񂪊܂܂�܂��B

#include "stdafx.h"



//added by takashi --------
#define LINE_SIZE 2048

void t_info(const char* fmt, ...)
{
	char line[LINE_SIZE];
	va_list ap;

	va_start(ap, fmt);
	vsprintf_s(line, LINE_SIZE, fmt, ap);
	va_end(ap);

	fprintf(stderr, "%s", line);
}
