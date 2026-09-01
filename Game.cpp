#include "Main.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
//#include "Audio.h"
#include "Input.h"
#include "Light.h"
#include "Camera.h"
#include "Texture.h"
#include "UI_Manager.h"

#include "Player.h"
#include "Environment_Objects.h"
#include "Item_Spawner.h"
#include "Skybox.h"


static int g_AudioBGM;
static PlayerCamera g_PlayerCamera;
static Player g_Player;

ItemSpawner g_ItemSpawner; // for extern use


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

	Skybox_Initialize();

	UIManager_Initialize();

	//g_AudioBGM = LoadAudio("Asset\\Audio\\bgm.wav");
	//PlayAudio(g_AudioBGM, true);
}

void Game_Finalize()
{
	//UnloadAudio(g_AudioBGM);

	//3D
	Skybox_Finalize();

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

	UIManager_Update(elapsed_Time);

	// Scene switch
	if (GetKeyTrigger(VK_RETURN))
	{
		SetScene(SCENE_RESULT);
	}
}

void Game_Draw()
{
	// 3D Drawing
	Renderer3D_Begin();

	Skybox_Draw();

	g_PlayerCamera.Bind();
	g_LightManager.BindAllLightsToPipeline();

	XMFLOAT3 cameraPos = g_PlayerCamera.GetPosition();

	EnvironmentObjects::Draw(cameraPos, g_PlayerCamera.GetView(), g_PlayerCamera.GetProj());
	g_ItemSpawner.Draw(cameraPos, g_PlayerCamera.GetView(), g_PlayerCamera.GetProj());
	g_Player.Draw(cameraPos);

	// 2D Drawing
	Renderer2D_Begin();
	UIManager_GameDraw();
}

