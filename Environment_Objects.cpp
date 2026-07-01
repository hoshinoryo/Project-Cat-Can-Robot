#include <vector>
#include <assert.h>
#include <cmath>
#include <cfloat>

#include "Environment_Objects.h"
#include "model_asset.h"
#include "model_renderer.h"
#include "Collision.h"

using namespace DirectX;

namespace EnvironmentObjects
{
	/*
	struct HeightPoint
	{
		float y = 0.0f;
		bool exists = false;
	};
	*/
	
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

	/*
	static long long MakeGridKey(int x, int z)
	{
		return (static_cast<long long>(x) << 32) ^ static_cast<unsigned int>(z);
	}
	*/

	// Ray casting
	static bool RayIntersectsTriangle(
		const XMVECTOR& rayOrigin,
		const XMVECTOR& rayDir,
		const XMVECTOR& v0,
		const XMVECTOR& v1,
		const XMVECTOR& v2,
		float& outT
	)
	{
		constexpr float EPSILON = 1e-6f;

		XMVECTOR edge1 = v1 - v0;
		XMVECTOR edge2 = v2 - v0;

		XMVECTOR pvec = XMVector3Cross(rayDir, edge2);
		float det = XMVectorGetX(XMVector3Dot(edge1, pvec));

		if (fabsf(det) < EPSILON)
		{
			return false;
		}

		float invDet = 1.0f / det;

		XMVECTOR tvec = rayOrigin - v0;
		float u = XMVectorGetX(XMVector3Dot(tvec, pvec)) * invDet;

		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		XMVECTOR qvec = XMVector3Cross(tvec, edge1);
		float v = XMVectorGetX(XMVector3Dot(rayDir, qvec)) * invDet;

		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		float t = XMVectorGetX(XMVector3Dot(edge2, qvec)) * invDet;

		if (t < 0.0f)
		{
			return false;
		}

		outT = t;
		return true;
	}

	void EnvironmentObject::Update(double elapsed_time)
	{
	}

	void EnvironmentObject::Draw(const XMFLOAT3& cameraPosition)
	{
		if (!m_Asset) return;

		XMMATRIX world = CreateWorldMatrix(*this);

		for (uint32_t meshIndex = 0; meshIndex < m_Asset->meshes.size(); meshIndex++)
		{
			ModelRenderer_Draw(
				m_Asset,
				meshIndex,
				world,
				cameraPosition
			);
		}
	}

	AABB EnvironmentObject::GetAABB() const
	{
		AABB aabb{};

		aabb.min = DirectX::XMFLOAT3(
			m_Position.x - 0.5f * m_Scale.x,
			m_Position.y - 0.5f * m_Scale.y,
			m_Position.z - 0.5f * m_Scale.z
		);

		aabb.max = DirectX::XMFLOAT3(
			m_Position.x + 0.5f * m_Scale.x,
			m_Position.y + 0.5f * m_Scale.y,
			m_Position.z + 0.5f * m_Scale.z
		);

		return aabb;
	}

	void Initialize()
	{
		g_EnvironmentObjects.clear();
		CollisionSystem::ClearColliders();

		// Import models
		ModelAsset* playField = ModelAsset_Load("Asset/Environment/PlayField.fbx");
		assert(playField);

		AddObject("playField", playField);
	}

	void Finalize()
	{
		g_EnvironmentObjects.clear();
	}

	void Update(double elapsed_time)
	{
		for (EnvironmentObject& object : g_EnvironmentObjects)
		{
			object.Update(elapsed_time);
		}
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
			object.Draw(cameraPosition);
		}
	}


	// Playfield raycasting
	// return collision point to outY
	bool GetPlayFieldY(float x, float z, float& outY)
	{
		constexpr float RAY_START_Y = 1000.0f;
		constexpr float RAY_MAX_DISTANCE = 2000.0f;

		XMVECTOR rayOrigin = XMVectorSet(x, RAY_START_Y, z, 1.0f);
		XMVECTOR rayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		bool hit = false;
		float nearestT = FLT_MAX;

		for (const EnvironmentObject& object : g_EnvironmentObjects)
		{
			if (object.m_Name != "playField") continue;
			if (!object.m_Asset) continue;

			XMMATRIX world = CreateWorldMatrix(object);

			for (uint32_t meshIndex = 0; meshIndex < object.m_Asset->meshes.size(); meshIndex++)
			{
				const MeshAsset& mesh = object.m_Asset->meshes[meshIndex];

				if (mesh.cpuVertices.empty() || mesh.cpuIndices.empty()) continue;

				for (size_t i = 0; i + 2 < mesh.cpuIndices.size(); i += 3)
				{
					uint32_t i0 = mesh.cpuIndices[i + 0];
					uint32_t i1 = mesh.cpuIndices[i + 1];
					uint32_t i2 = mesh.cpuIndices[i + 2];

					const XMFLOAT3& p0 = mesh.cpuVertices[i0].Position;
					const XMFLOAT3& p1 = mesh.cpuVertices[i1].Position;
					const XMFLOAT3& p2 = mesh.cpuVertices[i2].Position;

					XMVECTOR v0 = XMLoadFloat3(&p0);
					XMVECTOR v1 = XMLoadFloat3(&p1);
					XMVECTOR v2 = XMLoadFloat3(&p2);

					v0 = XMVector3TransformCoord(v0, world);
					v1 = XMVector3TransformCoord(v1, world);
					v2 = XMVector3TransformCoord(v2, world);

					float t = 0.0f;

					if (RayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, t))
					{
						if (t < nearestT && t <= RAY_MAX_DISTANCE)
						{
							nearestT = t;
							hit = true;
						}
					}
				}
			}
		}

		if (!hit)
		{
			return false;
		}

		XMVECTOR hitPos = rayOrigin + rayDir * nearestT;
		outY = XMVectorGetY(hitPos);

		return true;
	}
}

