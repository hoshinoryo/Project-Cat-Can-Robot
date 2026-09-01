/*==============================================================================

   Model asset drawing functions [model_renderer.cpp]
														 Author : Gu Anyi
														 Date   : 2026/01/28
--------------------------------------------------------------------------------

==============================================================================*/

#include "model_asset.h"
#include "model_renderer.h"
#include "model_shader.h"
#include "Renderer_Manager.h"
#include "Renderer3D.h"
//#include "unlit_shader.h"
//#include "default3Dmaterial.h"
#include "Texture.h"

using namespace DirectX;

//extern Default3DMaterial g_DefaultSceneMaterial;

static Texture g_TextureWhite;
static Texture g_NormalFlat;
static bool g_TexReady = false;
static ModelShader g_ModelShader;

static void BindPS_SRV(UINT slot, ID3D11ShaderResourceView* srv);
static ID3D11ShaderResourceView* FindSRV(ModelAsset* asset, const std::string& key);


void ModelRenderer_Initialize()
{
	if (g_TexReady) return;

	if (!g_ModelShader.Initialize()) return;

	// if texture is null
	if (!g_TextureWhite.Load(L"Asset/Texture/white.png"))
	{
		MessageBox(nullptr, "white.png load failed", "ModelRenderer Error", MB_OK);
		return;
	}

	if (!g_NormalFlat.Load(L"Asset/Texture/normal_flat.png"))
	{
		MessageBox(nullptr, "normal_flat.png load failed", "ModelRenderer Error", MB_OK);
		return;
	}

	g_TexReady = true;
}

void ModelRenderer_Finalize()
{
	if (!g_TexReady) return;

	g_ModelShader.Finalize();

	g_TextureWhite.Release();
	g_NormalFlat.Release();

	g_TexReady = false;
}

void ModelRenderer_Draw(
	ModelAsset* asset,
	uint32_t meshIndex,
	const XMMATRIX& world,
	const XMFLOAT3& cameraPos
)
{
	ModelRenderer_Initialize();

	if (!g_TexReady) return;
	if (!asset) return;
	if (meshIndex >= asset->meshes.size()) return;

	ID3D11DeviceContext* ctx = RendererManager_GetDeviceContext();
	if (!ctx) return;

	MeshAsset& mesh = asset->meshes[meshIndex];
	if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) return;

	g_ModelShader.Begin();
	g_ModelShader.SetWorldMatrix(world);

	ModelMaterial* mat = nullptr;
	if (mesh.materialIndex < asset->materials.size())
	{
		mat = &asset->materials[mesh.materialIndex];
	}
	if (mat)
	{
		g_ModelShader.SetColor(mat->BaseColor);
		g_ModelShader.SetSpecularParams(
			cameraPos,
			mat->SpecularPower,
			mat->SpecularColor
		);
	}
	else
	{
		g_ModelShader.SetColor({ 1, 1, 1, 1 });
		g_ModelShader.SetSpecularParams(cameraPos, 32.0f, { 1, 1, 1, 1 });
	}

	// Binding SRV
	ID3D11ShaderResourceView* diffuseSRV  = nullptr;
	ID3D11ShaderResourceView* normalSRV   = nullptr;
	ID3D11ShaderResourceView* specularSRV = nullptr;

	if (mat)
	{
		if (!mat->DiffuseMapPath.empty())
			diffuseSRV = FindSRV(asset, mat->DiffuseMapPath);
		if (!mat->NormalMapPath.empty())
			normalSRV = FindSRV(asset, mat->NormalMapPath);
		if (!mat->SpecularMapPath.empty())
			specularSRV = FindSRV(asset, mat->SpecularMapPath);
	}

	if (!diffuseSRV) diffuseSRV = g_TextureWhite.GetSRV();
	if (!normalSRV) normalSRV = g_NormalFlat.GetSRV();
	if (!specularSRV) specularSRV = g_TextureWhite.GetSRV();

	BindPS_SRV(0, diffuseSRV);
	BindPS_SRV(1, normalSRV);
	BindPS_SRV(2, specularSRV);

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Binding VB and IB
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	ctx->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	ctx->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//g_ShadowPass.BindShadowMapSRV(ctx);

	ctx->DrawIndexed(mesh.indexCount, 0, 0);
}

void ModelRenderer_UnlitDraw(
	ModelAsset* asset,
	uint32_t meshIndex,
	const XMMATRIX& world,
	const XMFLOAT4& color
)
{
	ModelRenderer_Initialize();

	if (!asset) return;
	if (meshIndex >= asset->meshes.size()) return;

	MeshAsset& mesh = asset->meshes[meshIndex];
	if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) return;
	if (mesh.skinned) return;

	g_ModelShader.Begin();
	g_ModelShader.SetWorldMatrix(world);
	g_ModelShader.SetColor(color);

	RendererManager_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Binding SRV
	ID3D11ShaderResourceView* diffuseSRV = nullptr;
	ModelMaterial* mat = nullptr;
	if (mesh.materialIndex < asset->materials.size())
	{
		mat = &asset->materials[mesh.materialIndex];
	}
	if (mat && !mat->DiffuseMapPath.empty())
	{
		diffuseSRV = FindSRV(asset, mat->DiffuseMapPath);
	}

	if (!diffuseSRV) diffuseSRV = g_TextureWhite.GetSRV();

	BindPS_SRV(0, diffuseSRV);

	ID3D11DeviceContext* ctx = RendererManager_GetDeviceContext();

	// Binding VB and IB
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	ctx->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	ctx->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	ctx->DrawIndexed(mesh.indexCount, 0, 0);
}

void ModelRenderer_DrawDepth(ModelAsset* asset, uint32_t meshIndex, const DirectX::XMMATRIX& world)
{
	ModelRenderer_Initialize();

	if (!g_TexReady) return;
	if (!asset) return;
	if (meshIndex >= asset->meshes.size()) return;

	MeshAsset& mesh = asset->meshes[meshIndex];
	if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) return;
	
	g_ModelShader.BeginDepthOnly();
	g_ModelShader.SetWorldMatrix(world);

	ID3D11DeviceContext* ctx = RendererManager_GetDeviceContext();
	
	ctx->PSSetShader(nullptr, nullptr, 0); // PS‚É–³Œø‰»
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	ctx->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	ctx->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	ctx->DrawIndexed(mesh.indexCount, 0, 0);
}

static void BindPS_SRV(UINT slot, ID3D11ShaderResourceView* srv)
{
	RendererManager_GetDeviceContext()->PSSetShaderResources(slot, 1, &srv);
}

static ID3D11ShaderResourceView* FindSRV(ModelAsset* asset, const std::string& key)
{
	if (!asset) return nullptr;
	if (key.empty()) return nullptr;

	auto findExact = [&](const std::string& k) -> ID3D11ShaderResourceView*
		{
			auto it = asset->textures.find(k);
			return (it == asset->textures.end() ? nullptr : it->second);
		};

	if (auto* srv = findExact(key)) return srv;

	std::string norm = key;
	for (auto& c : norm)
	{
		if (c == '\\') c = '/';
	}
	while (norm.rfind("./", 0) == 0)
		norm.erase(0, 2);
	if (auto* srv = findExact(norm)) return srv;

	size_t pos = norm.find_last_of('/');
	std::string base = (pos == std::string::npos) ? norm : norm.substr(pos + 1);
	if (auto* srv = findExact(base)) return srv;

	return nullptr;
}