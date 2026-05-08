#pragma once

#include <xaudio2.h>

void InitAudio();
void UninitAudio();


int LoadAudio(const char* FileName);
void UnloadAudio(int Index);
void PlayAudio(int Index, bool Loop = false);

