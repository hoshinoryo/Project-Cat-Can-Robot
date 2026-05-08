#pragma once
//field.h

enum PLAYER_MODE
{
	PLAYER_IDLE = 0,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_FALL,

};



struct PLAYER
{
	bool		Use;
	XMFLOAT3	Position;		//絶対座標（ゲーム空間での座標）
	XMFLOAT3	ViewPosition;	//相対座標（画面内座標）
	XMFLOAT3	DrawSize;		//表示用ポリゴンサイズ
	XMFLOAT3	CollisionSize;	//当たり判定用サイズ


	XMFLOAT3	Speed;	//	Float2 移動スピード
	XMFLOAT3	Kasoku;	//  Float2 加速度

	PLAYER_MODE	Mode;

	ID3D11ShaderResourceView* TexID;

};


void	InitPlayer();
void	UninitPlayer();
void	UpdatePlayer();
void	DrawPlayer();


PLAYER* GetPlayer();
