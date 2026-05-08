

#include "main.h"
#include "Renderer.h"
#include "Player.h"
#include "sprite.h"
#include "Input.h"
#include	"Block.h"
#include "collision.h"

//PCとSwitchは座標系が異なるので、このまま使えないから調整する
#define	START_POSITION_X ((SCREEN_WIDTH / 4))		//初期位置適当に（地面より上）
#define	START_POSITION_Y (SCREEN_HEIGHT-80.0f-40.0f)	//初期位置適当に
#define	PLAYER_SIZE_X	(80.0f)//サイズ適当に
#define	PLAYER_SIZE_Y	(80.0f)//サイズ適当に
#define	GROUND			(SCREEN_HEIGHT - 50.0f)//地面のY座標　初期位置より下で座標適当に
#define	GRAVITY			(9.8f/60.0f*6.0f)		//重力加速度　適当に

//	この座標範囲を出たらスクロースする
#define		SCROLL_LIMIT_UP		(SCREEN_HEIGHT/4)
#define		SCROLL_LIMIT_DOWN	(SCREEN_HEIGHT - (SCREEN_HEIGHT/4))
#define		SCROLL_LIMIT_LEFT	(SCREEN_WIDTH/4)
#define		SCROLL_LIMIT_RIGHT	(SCREEN_WIDTH - (SCREEN_WIDTH/4))



//bool	CollisionBlock();


PLAYER	Player;

void	PlayerIdle();
void	PlayerWalk();
void	PlayerJump();
void	PlayerFall();


PLAYER* GetPlayer()
{
	return &Player;
}

void	InitPlayer()
{

	ZeroMemory(&Player, sizeof(PLAYER));

	TexMetadata		metadata;
	ScratchImage	image;
	LoadFromWICFile(L"Asset\\Texture\\Block.png", WIC_FLAGS_NONE, &metadata, image);
	//読み込んだ画像データをDirectXへ渡してテクスチャとして管理させる
	CreateShaderResourceView(GetDevice(), image.GetImages(),
		image.GetImageCount(), metadata, &Player.TexID);
	//なんか失敗した場合に警告を出す
	assert(Player.TexID);

	Player.Use = true;
	Player.DrawSize = XMFLOAT3(PLAYER_SIZE_X, PLAYER_SIZE_Y,0);
	Player.CollisionSize = XMFLOAT3(PLAYER_SIZE_X * 0.8f, PLAYER_SIZE_Y * 0.8f,0);
	Player.Position = XMFLOAT3(START_POSITION_X, START_POSITION_Y, 0);
	Player.ViewPosition = XMFLOAT3(START_POSITION_X, START_POSITION_Y, 0);
	Player.Mode = PLAYER_IDLE;

	//
	Player.Speed = XMFLOAT3(0, 0, 0);					//MakeFloat2(0,0) 移動スピード初期化
	Player.Kasoku = XMFLOAT3(0, GRAVITY, 0);	//MakeFLoat2(0, 9,8f/60*6, 0) Y値のみ加速度適当に

	Player.Mode = PLAYER_FALL;

}
void	UninitPlayer()
{
	Player.TexID->Release();
}

void	UpdatePlayer()
{

	switch (Player.Mode)
	{
		case PLAYER_IDLE:
			PlayerIdle(); break;
		case PLAYER_WALK:
			PlayerWalk(); break;
		case PLAYER_JUMP:
			PlayerJump(); break;
		case PLAYER_FALL:
			PlayerFall(); break;
	}
	{
		XMFLOAT2	Scroll = GetScrollOffset();//現在のスクロール値
		Player.ViewPosition.x = Player.Position.x - Scroll.x;//	相対座標計算
		Player.ViewPosition.y = Player.Position.y - Scroll.y;

		float	ofsx = 0.0f;
		float	ofsy = 0.0f;
		if (Player.ViewPosition.x < SCROLL_LIMIT_LEFT)//相対座標がスクロールリミットを超えたか
		{
			ofsx = Player.ViewPosition.x - SCROLL_LIMIT_LEFT;//超えた場合は差分を計算
		}
		else if (Player.ViewPosition.x > SCROLL_LIMIT_RIGHT)
		{
			ofsx = Player.ViewPosition.x - SCROLL_LIMIT_RIGHT;
		}
		if (Player.ViewPosition.y < SCROLL_LIMIT_UP)
		{
			ofsy = Player.ViewPosition.y - SCROLL_LIMIT_UP;
		}
		else if (Player.ViewPosition.y > SCROLL_LIMIT_DOWN)
		{
			ofsy = Player.ViewPosition.y - SCROLL_LIMIT_DOWN;
		}

		UpdateScrollOffset(ofsx, ofsy);//差分の分だけスクロールさせる

	}

	return;



}
void	DrawPlayer()
{
	// マトリクス設定
	//SetWorldViewProjection2D();

	XMFLOAT2	position = XMFLOAT2(Player.ViewPosition.x, Player.ViewPosition.y);//相対座標で表示
	XMFLOAT2	size = XMFLOAT2(Player.DrawSize.x, Player.DrawSize.y);
	XMFLOAT4	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DrawSprite(false, Player.TexID, position, { 80.0f, 80.0f });//プレイヤーはスクロール無視
}

//bool	CollisionBlock()
//{
//	BLOCK* Block = GetBlock();
//
//	for (int i = 0; i < BLOCK_MAX; i++)
//	{
//		if (Player.Use && Block[i].Use)
//		{
//
//			//			bool hit = CheckBoxCollider(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
//			//			bool hit = CheckBoxColliderLeftBottom(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
//			bool hit = CheckBoxColliderRightTop(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
//			if (hit == true) return true;
//		}
//
//
//
//
//	}
//
//
//
//
//
//}



void	PlayerIdle()//停止状態
{

	Player.Speed.x = 0.0f;	//停止
	Player.Speed.y = 0.0f;	//停止

	if (GetKeyPress(VK_LEFT))		//左移動
	{
		Player.Mode = PLAYER_WALK;
	}
	else if (GetKeyPress(VK_RIGHT))//右移動
	{
		Player.Mode = PLAYER_WALK;
	}
	if (GetKeyTrigger('Z'))			//ジャンプ処理
	{
		Player.Speed.y = -20.0f;	//移動スピードセット
		Player.Kasoku.y = GRAVITY;	//加速度セット
		Player.Mode = PLAYER_JUMP;	//ジャンプへ移行
	}


}

void	PlayerWalk()//歩く
{
	if (GetKeyPress(VK_LEFT))		//左移動
	{
		Player.Speed.x = -5.0f;
	}
	else if (GetKeyPress(VK_RIGHT))//右移動
	{
		Player.Speed.x = 5.0f;
	}
	else
	{
		Player.Speed.x *= 0.8f;		//左右を押していない場合は速度を減衰
		if (fabs(Player.Speed.x) < 0.05f)
		{
			Player.Speed.x = 0.0f;	//停止したらIDLEへ移行
			Player.Mode = PLAYER_IDLE;
		}
	}

	//全ての足場と6点とのチェック
	BLOCK* Block = GetBlock();
	bool	hit1 = false;
	bool	hit2 = false;
	bool	hit3 = false;
	bool	hit4 = false;
	bool	hit5 = false;
	bool	hit6 = false;
	for (int i = 0; i < BLOCK_MAX; i++)
	{
		if (Player.Use && Block[i].Use)
		{
			hit1 |= CheckBoxColliderLeftBottom(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			hit2 |= CheckBoxColliderRightBottom(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			hit3 |= CheckBoxColliderLeftTop(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			hit4 |= CheckBoxColliderRightTop(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			hit5 |= CheckBoxColliderLeft(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			hit6 |= CheckBoxColliderRight(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
		}
	}
	if ((hit1 == false) && (hit2 == false))//完全に足場に乗っていない
	{
		Player.Speed.y = 0.0f;			//移動速度リセット
		Player.Kasoku.y = GRAVITY;		//加速度リセット
		Player.Mode = PLAYER_FALL;		//落下モードへ移行
	}

	if ((hit5 == true) && (Player.Speed.x < 0.0f))	//左にぶつかっている
	{
		Player.Speed.x = 0.0f;		//移動速度リセット
	}
	else if ((hit6 == true) && (Player.Speed.x > 0.0f))	//右にぶつかっている
	{
		Player.Speed.x = 0.0f;		//移動速度リセット
	}


	Player.Position.x += Player.Speed.x;	//座標の更新
	Player.Position.y += Player.Speed.y;
	Player.Position.z += Player.Speed.z;


	if (GetKeyTrigger('Z'))			//ジャンプ処理
	{
		Player.Speed.y = -20.0f;	//移動スピードセット
		Player.Kasoku.y = GRAVITY;	//加速度セット
		Player.Mode = PLAYER_JUMP;	//ジャンプへ移行
	}

}
void	PlayerJump()//ジャンプ中
{
	if (GetKeyPress(VK_LEFT))		//左移動
	{
		Player.Speed.x = -5.0f;
	}
	else if (GetKeyPress(VK_RIGHT))//右移動
	{
		Player.Speed.x = 5.0f;
	}
	else
	{
		Player.Speed.x *= 0.8f;		//左右を押していない場合は速度を減衰
		if (fabs(Player.Speed.x) < 0.05f)
		{
			Player.Speed.x = 0.0f;
		}
	}

	Player.Speed.y += Player.Kasoku.y;		//加速度をスピードへ加算

	if (Player.Speed.y > 0.0f)				//移動スピードが下へ向いた
	{
		Player.Mode = PLAYER_FALL;			//落下へ移行
	}

	Player.Position.x += Player.Speed.x;	//座標の更新
	Player.Position.y += Player.Speed.y;
	Player.Position.z += Player.Speed.z;

}
void	PlayerFall()//落下中
{

	if (GetKeyPress(VK_LEFT))		//左移動
	{
		Player.Speed.x = -5.0f;
	}
	else if (GetKeyPress(VK_RIGHT))//右移動
	{
		Player.Speed.x = 5.0f;
	}
	else
	{
		Player.Speed.x *= 0.8f;		//左右を押していない場合は速度を減衰
		if (fabs(Player.Speed.x) < 0.05f)
		{
			Player.Speed.x = 0.0f;
		}
	}

	Player.Speed.y += Player.Kasoku.y;		//加速度をスピードへ加算
	if (Player.Speed.y > 78.0f)
	{
		Player.Speed.y = 78.0f;	//加速限界
	}
	Player.Position.x += Player.Speed.x;	//座標の更新
	Player.Position.y += Player.Speed.y;
	Player.Position.z += Player.Speed.z;


	//足場チェック
	BLOCK* Block = GetBlock();
	for (int i = 0; i < BLOCK_MAX; i++)
	{
		if (Player.Use && Block[i].Use)
		{
			bool hit1 = CheckBoxColliderLeftBottom(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			bool hit2 = CheckBoxColliderRightBottom(Player.Position, Block[i].Position, Player.DrawSize, Block[i].DrawSize);
			if ( (hit1 == true)||(hit2==true))
			{	//足場の上にきっちり表示位置を補正する
				float	pos = -Player.DrawSize.y /2;
				pos += Block[i].Position.y - (Block[i].DrawSize.y / 2);
				Player.Position.y = pos;

				Player.Speed.y = 0.0f;		//移動速度リセット
				Player.Kasoku.y = 0.0f;		//加速度リセット

				Player.Mode = PLAYER_WALK;	//歩き状態へ移行
				break;
			}
		}
	}

	return;

}