#include "Main.h"
#include "UI_Manager.h"
#include "Renderer_Manager.h"
#include "Font_Drawer.h"
#include "Sprite.h"
#include "Texture.h"
#include "Item_Spawner.h"
#include "Manager.h"

extern ItemSpawner g_ItemSpawner;

namespace
{
	FontDrawer g_FontDrawer;
	Texture g_FontTexture;

	void DrawGameUIText()
	{
		// Item Counter
		std::string itemText = "Count  " + std::to_string(g_ItemSpawner.GetCollectedItemCount());
		g_FontDrawer.DrawContent(
			itemText.c_str(),
			{ SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.05f },
			1.0f,
			{ 1.0f, 1.0f, 1.0f, 1.0f }
		);

		// Time Counter
		int remainTime = static_cast<int>(GetGameTimeLimit() - GetSceneTime());
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

	/*
	void DrawDebugUI()
	{
	}
	*/
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

void UIManager_Draw()
{
	DrawGameUIText();
	//DrawDebugUI();
}
