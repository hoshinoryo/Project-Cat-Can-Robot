#include "Main.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "Audio.h"
#include "Input.h"


#include "Block.h"
#include "player.h"
#include "Bg.h"
#include "Camera.h"

static int g_AudioBGM;
static PlayerCamera g_PlayerCamera;


void InitGame()
{

	InitBlock();

	InitPlayer();
	InitBg();

	g_AudioBGM = LoadAudio("Asset\\Audio\\BGM.wav");
	//PlayAudio(g_AudioBGM, true);
}

void UninitGame()
{
	UnloadAudio(g_AudioBGM);
	UninitBg();

	UninitBlock();

	UninitPlayer();
}

void UpdateGame()
{
	UpdateBg();

	UpdateBlock();

	UpdatePlayer();


	if (GetKeyTrigger(VK_RETURN))
	{
		SetScene(SCENE_RESULT);
	}


}

void DrawGame()
{
	// 2D Drawing
	Renderer2D_Begin();
	DrawBg();
	//DrawField();
	DrawBlock();
	//DrawPiece();
	//DrawEffect();
	DrawPlayer();

	// 3D Drawing
	Renderer3D_Begin();
}
