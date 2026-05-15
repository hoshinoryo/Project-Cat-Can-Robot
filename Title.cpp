#include "Main.h"
#include "Renderer_Manager.h"
#include "Manager.h"
#include "Sprite.h"
#include "Title.h"
#include "Input.h"


static ID3D11ShaderResourceView* g_Texture{};

void InitTitle()
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"Asset\\Texture\\Title.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(
		RendererManager_GetDevice(),
		image.GetImages(),
		image.GetImageCount(),
		metadata,
		&g_Texture
	);
	assert(g_Texture);

}

void UninitTitle()
{
	g_Texture->Release();
}

void UpdateTitle()
{
	if (GetKeyTrigger(VK_RETURN))
	{
		SetScene(SCENE_GAME);
	}

}

void DrawTitle()
{

	DrawSprite(false,g_Texture,
				{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f },
				{ SCREEN_WIDTH, SCREEN_HEIGHT });
}
