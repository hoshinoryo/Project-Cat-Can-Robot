
#include "Main.h"
#include "Input.h"


BYTE g_OldKeyState[256]{};
BYTE g_KeyState[256]{};



void UpdateInput()
{

	memcpy( g_OldKeyState, g_KeyState, 256 );

	GetKeyboardState( g_KeyState );

}

bool GetKeyPress(BYTE KeyCode)
{
	return (g_KeyState[KeyCode] & 0x80);
}

bool GetKeyTrigger(BYTE KeyCode)
{
	return ((g_KeyState[KeyCode] & 0x80) && !(g_OldKeyState[KeyCode] & 0x80));
}
