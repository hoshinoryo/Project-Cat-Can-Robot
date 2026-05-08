#pragma once


//Bg.h

#include	"Main.h"

#define		BGSIZE_X	(SCREEN_WIDTH)
#define		BGSIZE_Y	(SCREEN_HEIGHT*2)


struct BGOBJECT
{

	XMFLOAT3	Position;
	ID3D11ShaderResourceView* TexID;

};

void	InitBg();
void	UninitBg();
void	UpdateBg();
void	DrawBg();


