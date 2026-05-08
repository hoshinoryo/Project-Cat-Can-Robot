#include "Main.h"
#include "Renderer.h"
#include "Sprite.h"
#include "Field.h"


static ID3D11ShaderResourceView* g_Texture{};

void InitField()
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"Asset\\Texture\\BG.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);

}

void UninitField()
{
	g_Texture->Release();
}

void UpdateField()
{
}

void DrawField()
{

	DrawSprite(g_Texture,
				{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f },
				{ SCREEN_WIDTH, SCREEN_HEIGHT });
}
