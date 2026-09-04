#include "Main.h"
#include "Manager.h"

// ---- Rendering setting ----
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Renderer_Manager.h"
// ---------------------------

#include "Sprite.h"
#include "Input.h"
#include "Audio.h"
#include "Mouse.h"

#include "Title.h"
#include "Game.h"
#include "Result.h"
#include "Item_Spawner.h"


extern ItemSpawner g_ItemSpawner;

static constexpr double GAME_TIME_LIMIT = 60.0;
static SCENE g_Scene = SCENE_NONE;
static double g_SceneTime = 0.0;
static int g_ResultItemCount = 0;


void InitManager()
{
	RendererManager_Initialize();
	Renderer3D_Initialize();
	Renderer2D_Initialize();

	Mouse_Initialize(GetWindow());
	
	InitSprite();
	InitAudio();

	// ---- START SCENE SETTING ----
	SetScene(SCENE_TITLE); // from game scene
}

void UninitManager()
{
	SetScene(SCENE_NONE);

	UninitAudio();
	UninitSprite();

	Mouse_Finalize();

	Renderer2D_Finalize();
	Renderer3D_Finalize();
	RendererManager_Finalize();
}

void UpdateManager(double elapsed_Time)
{
	UpdateInput();

	//g_SceneTime += elapsed_Time;

	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		g_SceneTime += elapsed_Time;

		UpdateTitle();
		break;

	case SCENE_GAME:
	{
		// Read the state before updating
		const bool wasPlaying = Game_IsPlaying();
		Game_Update(elapsed_Time);

		if (wasPlaying)
		{
			g_SceneTime += elapsed_Time;

			if (g_SceneTime >= GAME_TIME_LIMIT)
			{
				g_ResultItemCount = g_ItemSpawner.GetScore(); // copy the count to result
				SetScene(SCENE_RESULT);
			}
		}

		break;
	}
	case SCENE_RESULT:
		g_SceneTime += elapsed_Time;

		UpdateResult();
		break;

	default:
		break;
	}

}

void DrawManager()
{
	RendererManager_BeginFrame();

	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		Renderer2D_Begin();
		DrawTitle();
		break;

	case SCENE_GAME:
		
		Game_Draw();
		break;

	case SCENE_RESULT:
		Renderer2D_Begin();
		DrawResult();
		break;

	default:
		break;
	}


	RendererManager_EndFrame();
}



void SetScene(SCENE Scene)
{
	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		UninitTitle();
		break;

	case SCENE_GAME:
		Game_Finalize();
		break;

	case SCENE_RESULT:
		UninitResult();
		break;

	default:
		break;
	}


	g_Scene = Scene;
	g_SceneTime = 0.0;

	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		InitTitle();
		break;

	case SCENE_GAME:
		Game_Initialize();
		break;

	case SCENE_RESULT:
		InitResult();
		break;

	default:
		break;
	}


}

double Manager_GetSceneTime()
{
	return g_SceneTime;
}

double Manager_GetGameTimeLimit()
{
	return GAME_TIME_LIMIT;
}

int Manager_GetResultItemCount()
{
	return g_ResultItemCount;
}
