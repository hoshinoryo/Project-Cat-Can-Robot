

/*
	足場の処理が出来たら
	プレイヤーのモードと同じ考え方で
	ブロック種別番号を使って
	　１普通のブロック
	　２動くブロック
	　３トラップ（表示しないが触れている間は押し戻されるなどの邪魔をする）
	 などの種類を作る
	 ブロックの配置用配列に追加して、ブロック構造体内に番号を保存する


	 それぞれの処理は関数にしてUpdte()関数内でswitch文で関数に分岐させる。
	 それぞれ当たった時の処理が異なるので、これも処理を分岐させる。

*/


#include "main.h"
#include "Renderer.h"
#include "Block.h"
#include "sprite.h"


BLOCK	Block[BLOCK_MAX];

ID3D11ShaderResourceView* g_Texture[10]; 

BLOCK_MAP	BlockMap[] =
{	//サンプルなので配置やサイズは適当
	{0,XMFLOAT3(SCREEN_WIDTH/2,SCREEN_HEIGHT-40,0.0f), XMFLOAT3(80.0f * 15, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f,SCREEN_HEIGHT - 40 - 80*2,0.0f), XMFLOAT3(80.0f * 4, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f,SCREEN_HEIGHT - 40 - 80 * 4,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f,SCREEN_HEIGHT - 40 - 80 * 6,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f-80*1,SCREEN_HEIGHT - 40 - 80 * 8,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f-80*2,SCREEN_HEIGHT - 40 - 80 * 10,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f-80*3,SCREEN_HEIGHT - 40 - 80 * 12,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},

	{0,XMFLOAT3(900.0f-80*3,SCREEN_HEIGHT - 40 - 80 * 14,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 16,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 18,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 20,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 22,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},

	{0,XMFLOAT3(900.0f - 80 * 0,SCREEN_HEIGHT - 40 - 80 * 24,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 26,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 0,SCREEN_HEIGHT - 40 - 80 * 28,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 30,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 0,SCREEN_HEIGHT - 40 - 80 * 32,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},

	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 34,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 0,SCREEN_HEIGHT - 40 - 80 * 36,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 3,SCREEN_HEIGHT - 40 - 80 * 38,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 4,SCREEN_HEIGHT - 40 - 80 * 40,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 6,SCREEN_HEIGHT - 40 - 80 * 42,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 4,SCREEN_HEIGHT - 40 - 80 * 44,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},
	{0,XMFLOAT3(900.0f - 80 * 6,SCREEN_HEIGHT - 40 - 80 * 46,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},

//	{3,XMFLOAT3(900.0f - 80 * 8,SCREEN_HEIGHT - 40 - 80 * 48,0.0f), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},




//	{0,XMFLOAT3(500.0f,SCREEN_HEIGHT - 40 - 80,0), XMFLOAT3(80.0f * 2, 80.0f,0.0f)},


	{0,XMFLOAT3(-999.0f,500.0f,0.0f), XMFLOAT3(80.0f, 80.0f,0.0f)},

};

BLOCK* GetBlock()
{
	return &Block[0];
}


void	InitBlock()
{

	ZeroMemory(&g_Texture[0], sizeof(ID3D11ShaderResourceView*) * 10);


	TexMetadata		metadata;
	ScratchImage	image;
	LoadFromWICFile(L"Asset\\Texture\\Block.png", WIC_FLAGS_NONE, &metadata, image);
	//読み込んだ画像データをDirectXへ渡してテクスチャとして管理させる
	CreateShaderResourceView(GetDevice(), image.GetImages(),
		image.GetImageCount(), metadata, &g_Texture[0]);
	//なんか失敗した場合に警告を出す
	assert(g_Texture[0]);

	LoadFromWICFile(L"Asset\\Texture\\Goal.jpg", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(),
		image.GetImageCount(), metadata, &g_Texture[1]);
	//なんか失敗した場合に警告を出す
	assert(g_Texture[1]);


	ZeroMemory(&Block[0], sizeof(BLOCK) * BLOCK_MAX);

	for(int i = 0; i < BLOCK_MAX; i++)
	{
		if (BlockMap[i].Position.x == -999.0f) break;
		if (Block[i].Use == false)
		{
			Block[i].Position = BlockMap[i].Position;
			Block[i].DrawSize = BlockMap[i].DrawSize;
			Block[i].Use = true;
			Block[i].No = BlockMap[i].no;	//ブロック種別
			Block[i].TexID = g_Texture[0];
		}
	}


}
void	UninitBlock()
{
	g_Texture[0]->Release();
	g_Texture[1]->Release();
}

void	UpdateBlock()
{

	for (int i = 0; i < BLOCK_MAX; i++)
	{
		if (Block[i].Use == true)
		{
			switch (Block[i].No)//ブロックの種類ごとに処理
			{
			case BLOCK_NORMAL:	//普通の足場
			case BLOCK_MOVE:	//動く足場
			case BLOCK_TRAP_0:	//不可視罠0
				break;
			}
		}

	}




}
void	DrawBlock()
{
	// マトリクス設定
	//SetWorldViewProjection2D();

	XMFLOAT2	texcoord(1.0f / 8 * 1, 1.0f / 4 * 1);
	XMFLOAT2	texsize(1.0f / 8 * 1, 1.0f / 4 * 1);

	for (int i = 0; i < BLOCK_MAX; i++)
	{
		if (Block[i].Use == true)
		{
			XMFLOAT2	scroll = GetScrollOffset();//現在のスクロール値
			Block[i].ViewPosition.x = Block[i].Position.x-scroll.x;//相対座標を計算(一応保存)
			Block[i].ViewPosition.y = Block[i].Position.y-scroll.y;
	
			XMFLOAT2	position = XMFLOAT2(Block[i].Position.x, Block[i].Position.y);
			//			XMFLOAT2	size = XMFLOAT2(Block[i].Position.x, Block[i].Position.y);
			XMFLOAT4	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			if (Block[i].No == 0)
			{
				DrawSpriteTexCoord(true, Block[i].TexID, position, { Block[i].DrawSize.x   , Block[i].DrawSize.y }, texcoord, texsize);
			}
			else
			{
				DrawSprite(true, Block[i].TexID, position, { Block[i].DrawSize.x   , Block[i].DrawSize.y });
			}
		}
	}

}

