/*==============================================================================

   ‹ó‚Ì•`‰æ [skybox.h]
														 Author : Gu Anyi
														 Date   : 2025/11/21

--------------------------------------------------------------------------------

==============================================================================*/

#ifndef SKYDOME_H
#define SKYDOME_H

#include <DirectXMath.h>

void Skybox_Initialize();
void Skybox_Finalize();
void Skybox_SetPosition(const DirectX::XMFLOAT3& position);
void Skybox_Draw();

#endif // SKYDOME_H
