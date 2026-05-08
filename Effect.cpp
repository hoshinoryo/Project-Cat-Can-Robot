#include "Main.h"
#include "Renderer.h"
#include "Sprite.h"
#include "Effect.h"


static ID3D11ShaderResourceView* g_Texture{};


#define EFFECT_MAX 100
static EFFECT g_Effect[EFFECT_MAX]{};




void InitEffect()
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;

	LoadFromWICFile(L"Asset\\Texture\\Explosion.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);




	for (int i = 0; i < EFFECT_MAX; i++)
	{
		g_Effect[i].Enable = false;
	}

}

void UninitEffect()
{
	g_Texture->Release();

}

void UpdateEffect()
{
	//アニメーション処理
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (g_Effect[i].Enable)
		{
			g_Effect[i].FrameCount++;

			if (g_Effect[i].FrameCount > 30)
			{
				g_Effect[i].Enable = false;
			}
		}
	}
}

void DrawEffect()
{

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (g_Effect[i].Enable)
		{
			int frame = (int)(g_Effect[i].FrameCount / 30.0f * 16.0f);

			XMFLOAT2 texPosition;
			texPosition.x = frame % 4 / 4.0f;
			texPosition.y = frame / 4 / 4.0f;

			DrawSpriteTexCoord(g_Texture,
								g_Effect[i].Position,
								{ PIECE_WIDTH, PIECE_HEIGHT },
								texPosition,
								{1.0f / 4.0f, 1.0f / 4.0f } );
		}
	}
}



void CreateEffect(XMFLOAT2 Position)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (g_Effect[i].Enable == false)
		{
			g_Effect[i].Enable = true;
			g_Effect[i].Position = Position;
			g_Effect[i].FrameCount = 0;

			break;
		}
	}
}
