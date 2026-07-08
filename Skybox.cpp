/*==============================================================================

   ‹ó‚Ì•`‰æ [skybox.cpp]
														 Author : Gu Anyi
														 Date   : 2025/11/21

--------------------------------------------------------------------------------

==============================================================================*/

#include "Skybox.h"
#include "model_asset.h"
#include "model_renderer.h"
#include "Renderer3D.h"

#include <DirectXMath.h>

using namespace DirectX;

static ModelAsset* g_pModelSky{ nullptr };
static XMFLOAT3 g_Position{};


void Skybox_Initialize()
{
	g_pModelSky = ModelAsset_Load("Asset/Environment/Skybox.fbx", true, 1.0f);
}

void Skybox_Finalize()
{
	ModelAsset_Release(g_pModelSky);
}

void Skybox_SetPosition(const DirectX::XMFLOAT3& position)
{
	g_Position = position;
}

void Skybox_Draw()
{
	Renderer3D_BeginSkydome();

	const XMMATRIX instanceWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&g_Position));

	for (uint32_t mi = 0; mi < (uint32_t)g_pModelSky->meshes.size(); mi++)
	{
		const XMMATRIX nodeToModel = XMLoadFloat4x4(&g_pModelSky->meshes[mi].nodeToModel);
		const XMMATRIX world = nodeToModel * instanceWorld;

		ModelRenderer_UnlitDraw(g_pModelSky, mi, world, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	Renderer3D_EndSkydome();
}
