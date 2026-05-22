#include <vector>
#include <assert.h>

#include "Environment_Objects.h"
#include "model_asset.h"
#include "model_renderer.h"

using namespace DirectX;

namespace EnvironmentObjects
{
	struct EnvironmentObject
	{
		std::string m_Name;
		ModelAsset* m_Asset{};

		XMFLOAT3 m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 m_Scale    = XMFLOAT3(1.0f, 1.0f, 1.0f);
	};

	static std::vector<EnvironmentObject> g_EnvironmentObjects;

	static XMMATRIX CreateWorldMatrix(const EnvironmentObject& object)
	{
		XMMATRIX scale = XMMatrixScaling(
			object.m_Scale.x,
			object.m_Scale.y,
			object.m_Scale.z
		);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(
			object.m_Rotation.x,
			object.m_Rotation.y,
			object.m_Rotation.z
		);
		XMMATRIX translation = XMMatrixTranslation(
			object.m_Position.x,
			object.m_Position.y,
			object.m_Position.z);

		return scale * rotation * translation;
	}

	void Initialize()
	{
		g_EnvironmentObjects.clear();

		// Import models
		ModelAsset* playField = ModelAsset_Load("Asset/Environment/PlayField.fbx");
		assert(playField);

		AddObject("playField", playField);
	}

	void Finalize()
	{
		g_EnvironmentObjects.clear();
	}

	void AddObject(const std::string& name, ModelAsset* asset, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale)
	{
		if (!asset) return;

		EnvironmentObject object;
		object.m_Name = name;
		object.m_Asset = asset;
		object.m_Position = position;
		object.m_Rotation = rotation;
		object.m_Scale = scale;

		g_EnvironmentObjects.push_back(object);
	}

	void Draw(const DirectX::XMFLOAT3& cameraPosition)
	{
		for (EnvironmentObject& object : g_EnvironmentObjects)
		{
			if (!object.m_Asset) continue;

			XMMATRIX world = CreateWorldMatrix(object);

			for (uint32_t meshIndex = 0; meshIndex < object.m_Asset->meshes.size(); meshIndex++)
			{
				ModelRenderer_Draw(
					object.m_Asset,
					meshIndex,
					world,
					cameraPosition
				);
			}
		}
	}
}


