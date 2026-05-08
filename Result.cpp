#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Result.h"
#include "Input.h"


static ID3D11ShaderResourceView* g_Texture{};

void InitResult()
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"Asset\\Texture\\Result.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);

}

void UninitResult()
{
	g_Texture->Release();
}

void UpdateResult()
{
	if (GetKeyTrigger(VK_RETURN))
	{
		SetScene(SCENE_TITLE);
	}

}

void DrawResult()
{

	DrawSprite(false, g_Texture,
				{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f },
				{ SCREEN_WIDTH, SCREEN_HEIGHT });
}
