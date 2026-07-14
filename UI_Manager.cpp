#include "Main.h"
#include "UI_Manager.h"
#include "Renderer_Manager.h"
#include "Font_Drawer.h"
#include "Sprite.h"
#include "Texture.h"
#include "Item_Spawner.h"
#include "Manager.h"

extern ItemSpawner g_ItemSpawner;

// static
namespace
{
	FontDrawer g_FontDrawer;
	Texture g_FontTexture;
}


void UIManager_Initialize()
{
	g_FontTexture.Load(L"Asset\\Font\\chi_font_0.png");
	g_FontDrawer.LoadContent("Asset\\Font\\chi_font.fnt", g_FontTexture.GetSRV());
}

void UIManager_Finalize()
{
}

void UIManager_Update(double elapsed_time)
{
}

void UIManager_GameDraw()
{
	// Item score
	std::string itemText = "Score  " + std::to_string(g_ItemSpawner.GetScore());
	g_FontDrawer.DrawContent(
		itemText.c_str(),
		{ SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.05f },
		1.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	);

	// Time Counter
	int remainTime = static_cast<int>(Manager_GetGameTimeLimit() - Manager_GetSceneTime());
	if (remainTime < 0)
	{
		remainTime = 0;
	}

	std::string timeText = "Time  " + std::to_string(remainTime);
	g_FontDrawer.DrawContent(
		timeText.c_str(),
		{ SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.05f },
		1.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	);
}

void UIManager_ResultDraw()
{
	// Score
	std::string scoreText = std::to_string(Manager_GetResultItemCount());
	g_FontDrawer.DrawContent(
		scoreText.c_str(),
		{ SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.35f },
		8.0f,
		{ 0.2f, 0.2f, 0.2f, 1.0f }
	);
}
