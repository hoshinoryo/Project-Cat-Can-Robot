#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "Audio.h"
#include "Input.h"


#include "Block.h"
#include "player.h"
#include "Bg.h"

static int g_AudioBGM;


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
	DrawBg();
	//DrawField();
	DrawBlock();
	//DrawPiece();
	//DrawEffect();
	DrawPlayer();

}
