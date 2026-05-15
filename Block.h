#pragma once
//field.h

#include "Renderer2D.h"
#include "main.h"


#define		BLOCK_MAX	(100)


enum BLOCK_NO
{
	BLOCK_NORMAL = 0,	//普通のブロック
	BLOCK_MOVE,			//動くブロック
	BLOCK_TRAP_0,		//罠系

};

struct BLOCK
{
	bool		Use;
	int			No;
	XMFLOAT3	Position;		//絶対座標（ゲーム空間での座標）
	XMFLOAT3	ViewPosition;	//相対座標（画面内座標）
	XMFLOAT3	DrawSize;		//表示用ポリゴンサイズ
	XMFLOAT3	CollisionSize;	//当たり判定用サイズ
	ID3D11ShaderResourceView* TexID;

};


struct BLOCK_MAP
{
	int			no;
	XMFLOAT3	Position;
	XMFLOAT3	DrawSize;

};


void	InitBlock();
void	UninitBlock();
void	UpdateBlock();
void	DrawBlock();

BLOCK* GetBlock();


