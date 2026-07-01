#include "Main.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "Audio.h"
#include "Input.h"
#include "Light.h"
#include "Camera.h"
#include "Texture.h"

#include "Player.h"
#include "Environment_Objects.h"
#include "Item_Spawner.h"
#include "Font_Drawer.h"


static int g_AudioBGM;
static PlayerCamera g_PlayerCamera;
static Player g_Player;
static ItemSpawner g_ItemSpawner;
static FontDrawer g_FontDrawer;
static Texture g_FontTexture;


void Game_Initialize()
{
	// 3D
	g_PlayerCamera.Initialize();

	g_LightManager.Initialize();
	g_LightManager.SetAmbient(DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	g_LightManager.SetDirectionalWorld(DirectX::XMFLOAT4(0.3f, -1.0f, 0.3f, 0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//g_LightManager.SetPointLightCount(1);
	//g_LightManager.SetPointLight(0, DirectX::XMFLOAT3(0.0f, 3.0f, 0.0f), 8.0f, DirectX::XMFLOAT3(1.0f, 0.8f, 0.6f));

	EnvironmentObjects::Initialize();
	g_Player.Initialize(XMFLOAT3(0.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	g_ItemSpawner.Initialize();
	g_FontTexture.Load(L"Asset\\Font\\chi_font_0.png");
	g_FontDrawer.LoadContent("Asset\\Font\\chi_font.fnt", g_FontTexture.GetSRV());

	g_AudioBGM = LoadAudio("Asset\\Audio\\BGM.wav");
	//PlayAudio(g_AudioBGM, true);
}

void Game_Finalize()
{
	UnloadAudio(g_AudioBGM);

	//3D
	g_ItemSpawner.Finalize();
	g_Player.Finalize();
	EnvironmentObjects::Finalize();
	g_LightManager.Finalize();
	g_PlayerCamera.Finalize();
}

void Game_Update(double elapsed_Time)
{
	// 3D
	g_Player.Update(elapsed_Time, g_PlayerCamera.GetFront());
	g_PlayerCamera.SetFollowTarget(&g_Player.GetPosition());
	g_PlayerCamera.Update(elapsed_Time);

	EnvironmentObjects::Update(elapsed_Time);
	g_ItemSpawner.Update(elapsed_Time, g_Player);

	// Scene switch
	if (GetKeyTrigger(VK_RETURN))
	{
		SetScene(SCENE_RESULT);
	}
}

void Game_Draw()
{
	Renderer3D_Begin();

	g_PlayerCamera.Bind();
	g_LightManager.BindAllLightsToPipeline();

	XMFLOAT3 cameraPos = g_PlayerCamera.GetPosition();

	EnvironmentObjects::Draw(cameraPos);
	g_ItemSpawner.Draw(cameraPos);
	g_Player.Draw(cameraPos);

	Renderer2D_Begin();

	g_FontDrawer.DrawContent(
		"test text",
		{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f },
		1.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	);
}

