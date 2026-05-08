#pragma once




struct EFFECT
{
	bool Enable;
	XMFLOAT2 Position;
	int FrameCount;
};



void InitEffect();
void UninitEffect();
void UpdateEffect();
void DrawEffect();


void CreateEffect(XMFLOAT2 Position);



