/*----------------------------------------------------------------------------
    SproutViewer4D: 4DCT analysis software
	Copyright (C) 2017, Takashi  Ijiri 
	Copyright (C) 2017. Tomofumi Narita
	
	This program is released under GNU GPL v3.
	See README and LICENSE for detail.
----------------------------------------------------------------------------*/



// stdafx.cpp : 標準インクルード SproutViewer.pch のみを
// 含むソース ファイルは、プリコンパイル済みヘッダーになります。
// stdafx.obj にはプリコンパイル済み型情報が含まれます。

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
