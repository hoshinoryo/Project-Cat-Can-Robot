/*==============================================================================

   Model asset import and release [model_asset.h]
														 Author : Gu Anyi
														 Date   : 2026/01/28
--------------------------------------------------------------------------------

==============================================================================*/

#ifndef MODEL_ASSET_H
#define MODEL_ASSET_H

// --------------------
// Assimp lib
// --------------------
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/config.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include <vector>
#include <unordered_map>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Collision.h"
#include "Renderer3D.h"

class Default3DMaterial;

// Material
struct ModelMaterial
{
	std::string Name;

	DirectX::XMFLOAT4 BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 SpecularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float SpecularPower = 32.0f;

	std::string DiffuseMapPath;
	std::string NormalMapPath;
	std::string SpecularMapPath;

	ID3D11ShaderResourceView* DiffuseSRV = nullptr;
	ID3D11ShaderResourceView* NormalSRV = nullptr;
	ID3D11ShaderResourceView* SpecularSRV = nullptr;
};

// aiMeshÇ≤Ç∆Ç…ä«óùÇ≥ÇÍÇƒÇÈ
struct MeshAsset
{
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	uint32_t indexCount = 0;
	uint32_t materialIndex = 0;

	bool skinned = false;
	AABB localAABB{};

	DirectX::XMFLOAT4X4 nodeToModel{}; // accumulated matrix

	// CPU resources
	std::vector<VERTEX_3D> cpuVertices;
	std::vector<uint32_t> cpuIndices;
};

// fbxÉtÉ@ÉCÉãÇ≤Ç∆Ç…ä«óùÇ≥ÇÍÇƒÇ¢ÇÈ
struct ModelAsset
{
	// Import settings
	float importScale = 1.0f;
	bool sourceYup = true;

	DirectX::XMMATRIX importFix = DirectX::XMMatrixIdentity();

	// Assimp assets
	const aiScene* aiScene = nullptr;

	// GPU resources and materials
	std::vector<MeshAsset> meshes;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> textures;
	//std::vector<Default3DMaterial*> materials;
	std::vector<ModelMaterial> materials;

	// Bones
	std::unordered_map<std::string, int> boneNameToIndex;
};

ModelAsset* ModelAsset_Load(const char* filename, bool yUp = false, float scale = 1.0f);
void        ModelAsset_Release(ModelAsset* asset);

#endif // MODEL_ASSET_H
