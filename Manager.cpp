#include "Main.h"
#include "Manager.h"
#include "Renderer.h"
#include "Sprite.h"
#include "Input.h"
#include "Audio.h"

#include "Title.h"
#include "Game.h"
#include "Result.h"

static SCENE g_Scene = SCENE_NONE;




void InitManager()
{
	InitRenderer();
	InitSprite();
	InitAudio();


	SetScene(SCENE_TITLE);
}

void UninitManager()
{
	SetScene(SCENE_NONE);


	UninitAudio();
	UninitSprite();
	UninitRenderer();
}

void UpdateManager()
{
	UpdateInput();

	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		UpdateTitle();
		break;

	case SCENE_GAME:
		UpdateGame();
		break;

	case SCENE_RESULT:
		UpdateResult();
		break;

	default:
		break;
	}

}

void DrawManager()
{
	BeginRenderer();

	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		DrawTitle();
		break;

	case SCENE_GAME:
		DrawGame();
		break;

	case SCENE_RESULT:
		DrawResult();
		break;

	default:
		break;
	}


	EndRenderer();
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
		UninitGame();
		break;

	case SCENE_RESULT:
		UninitResult();
		break;

	default:
		break;
	}


	g_Scene = Scene;


	switch (g_Scene)
	{
	case SCENE_NONE:
		break;

	case SCENE_TITLE:
		InitTitle();
		break;

	case SCENE_GAME:
		InitGame();
		break;

	case SCENE_RESULT:
		InitResult();
		break;

	default:
		break;
	}


}