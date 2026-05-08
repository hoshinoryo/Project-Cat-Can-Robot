#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Piece.h"
#include "Block.h"
#include "Input.h"
#include "Audio.h"


static ID3D11ShaderResourceView* g_Texture[4]{};

struct PIECE
{
	XMFLOAT2 Position;
	int Type[3];
};

static PIECE g_Piece{};



enum PIECE_STATE
{
	PIECE_STATE_IDLE,	//ひま
	PIECE_STATE_MOVE,	//落下中
	PIECE_STATE_GROUND_IDLE,	//着地中
	PIECE_STATE_MISS_IDLE,		//ミス中
};

static PIECE_STATE g_PieceState{};
static int g_PieceStateCount{};


static int g_PieceAudio{};


void InitPiece()
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;

	LoadFromWICFile(L"Asset\\Texture\\Spade.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[0]);
	assert(g_Texture[0]);

	LoadFromWICFile(L"Asset\\Texture\\Clover.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[1]);
	assert(g_Texture[1]);

	LoadFromWICFile(L"Asset\\Texture\\Heart.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[2]);
	assert(g_Texture[2]);

	LoadFromWICFile(L"Asset\\Texture\\Diamond.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[3]);
	assert(g_Texture[3]);



	g_PieceAudio = LoadAudio("Asset\\Audio\\wan.wav");

	CreatePiece();
}



void UninitPiece()
{
	UnloadAudio(g_PieceAudio);



	for (int i = 0; i < 4; i++)
	{
		g_Texture[i]->Release();
	}
}



void UpdatePiece()
{

	switch (g_PieceState)
	{
	case PIECE_STATE_IDLE:	//ひま
		break;

	case PIECE_STATE_MOVE:	//落下中
		MovePiece();
		break;

	case PIECE_STATE_GROUND_IDLE://着地中
		g_PieceStateCount++;
		if (g_PieceStateCount > 60)//接地待ち
		{
			g_PieceState = PIECE_STATE_IDLE;//ひまに移行
			g_PieceStateCount = 0;

			//EraseBlock();//ブロック消し
		}
		break;

	case PIECE_STATE_MISS_IDLE://ミス
		g_PieceStateCount++;
		if (g_PieceStateCount > 60)//ウェイト
		{
			SetScene(SCENE_RESULT);
		}
		break;

	default:
		break;
	}

}





void CreatePiece()
{
	g_Piece.Position = { SCREEN_WIDTH * 0.5f + PIECE_WIDTH * 0.5f, 0.0f };//出現位置
	for (int i = 0; i < 3; i++)
	{
		g_Piece.Type[i] = rand() % 4;//3つの種類を決める
	}

	//一番下のピースの配列内の位置を計算
	// (posX-Offset) / PICE_WIDTH
	// (posY-Offset + (PICE_HEIGHT*3)) / PICE_HEIGHT
	int x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
	int y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

	//BLOCK block = GetBlock(x, y);	//ブロック情報取得

	//if (block.Enable)//発生時に一番下のピースがブロックにめり込んでる場合はミス
	//{
	//	g_PieceState = PIECE_STATE_MISS_IDLE;
	//	g_PieceStateCount = 0;
	//}
	//else//そうでなければ落下開始
	//{
	//	g_PieceState = PIECE_STATE_MOVE;
	//	g_PieceStateCount = 0;
	//}

}


void MovePiece()
{

	if (GetKeyTrigger(VK_UP))//ピースのローテーション
	{
		int type = g_Piece.Type[0];

		for (int i = 0; i < 2; i++)
			g_Piece.Type[i] = g_Piece.Type[i + 1];

		g_Piece.Type[2] = type;
	}




	int x, y;
	BLOCK block;


	y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

	//左移動
	if (GetKeyTrigger(VK_LEFT))
	{	//Xの配列位置
		x = (int)((g_Piece.Position.x - PIECE_WIDTH - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);

		if (x >= 0)//配列内にいる
		{
			block = GetBlock(x, y);

			if (block.Enable == false)//隣にブロックが無ければ左へ移動
				g_Piece.Position.x -= PIECE_WIDTH;
		}
	}
	//右移動
	if (GetKeyTrigger(VK_RIGHT))
	{
		x = (int)((g_Piece.Position.x + PIECE_WIDTH - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);

		if (x < BLOCK_COLS)
		{
			block = GetBlock(x, y);

			if (block.Enable == false)//隣にブロックが無ければ右へ移動
				g_Piece.Position.x += PIECE_WIDTH;
		}
	}

	if (GetKeyPress(VK_DOWN))
	{
		g_Piece.Position.y += 5.0f;//加速装置落下
	}


	g_Piece.Position.y += 2.0f;	//デフォルト落下




	//着地チェック

	x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
	y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

	block = GetBlock(x, y);




	bool ground = false;

	if (block.Enable)//ブロックの上に載った
	{
		ground = true;
	}
	else if (g_Piece.Position.y + PIECE_HEIGHT * 2 > SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT)
	{//底に着地した y>=13
		ground = true;
	}


	if (ground)//着地してたら
	{
		for (int i = 0; i < 3; i++)
		{
			//配列にピースを移植
			x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
			y = (int)((g_Piece.Position.y + PIECE_HEIGHT * i - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

			SetBlock(x, y, g_Piece.Type[i]);
		}


		PlayAudio(g_PieceAudio);


		//接地待ちへ移行
		g_PieceState = PIECE_STATE_GROUND_IDLE;
		g_PieceStateCount = 0;

	}
}




void DrawPiece()
{
	//移動中でもなく、ミス中でもない場合は何もしない
	if (g_PieceState != PIECE_STATE_MOVE && g_PieceState != PIECE_STATE_MISS_IDLE)
		return;


	for (int i = 0; i < 3; i++)
	{
		//上から順番に3つ表示
		DrawSprite(g_Texture[g_Piece.Type[i]],
					{ g_Piece.Position.x, g_Piece.Position.y + PIECE_HEIGHT * i },
					{ PIECE_WIDTH, PIECE_HEIGHT });
	}
}
