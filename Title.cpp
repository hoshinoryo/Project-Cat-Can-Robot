#include "Main.h"
#include "Renderer_Manager.h"
#include "Manager.h"
#include "Sprite.h"
#include "Title.h"
#include "Input.h"
#include "Audio.h"


static ID3D11ShaderResourceView* g_Texture{};
static int g_TitleBgm = -1;

void InitTitle()
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	HRESULT hr = LoadFromWICFile(
		L"Asset/Texture/Title.png",
		WIC_FLAGS_NONE,
		&metadata,
		image
	);
	assert(SUCCEEDED(hr));

	hr = CreateShaderResourceView(
		RendererManager_GetDevice(),
		image.GetImages(),
		image.GetImageCount(),
		metadata,
		&g_Texture
	);
	assert(SUCCEEDED(hr));
	assert(g_Texture);

	g_TitleBgm = LoadAudio("Asset/Audio/Cat_Youtube_Star.wav");
	//if (g_TitleBgm != -1)
	//{
		PlayAudio(g_TitleBgm, true);
	//}
}

void UninitTitle()
{
	if (g_TitleBgm != -1)
	{
		UnloadAudio(g_TitleBgm);
		g_TitleBgm = -1;
	}

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
	DrawSprite(false, g_Texture,
				{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f },
				{ SCREEN_WIDTH, SCREEN_HEIGHT });
}
