#include <vector>
#include <assert.h>
#include <cmath>
#include <unordered_map>
#include <cfloat>

#include "Environment_Objects.h"
#include "model_asset.h"
#include "model_renderer.h"
#include "Collision.h"

using namespace DirectX;

namespace EnvironmentObjects
{
	// Playfield collision AABB
	static constexpr float PLAYFIELD_GRID_SIZE = 1.0f;
	static constexpr float PLAYFIELD_COLLIDER_THICKNESS = 1.0f;


	struct EnvironmentObject
	{
		std::string m_Name;
		ModelAsset* m_Asset{};

		XMFLOAT3 m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 m_Scale    = XMFLOAT3(1.0f, 1.0f, 1.0f);
	};

	struct HeightPoint
	{
		float y = 0.0f;
		bool exists = false;
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

	static long long MakeGridKey(int x, int z)
	{
		return (static_cast<long long>(x) << 32) ^ static_cast<unsigned int>(z);
	}
	
	/*
	static void AddPlayFieldGridAABBs(const EnvironmentObject& object)
	{
		if (!object.m_Asset || !object.m_Asset->aiScene) return;

		std::unordered_map<long long, HeightPoint> heightMap;

		int minGridX = INT_MAX;
		int maxGridX = -INT_MAX;
		int minGridZ = INT_MAX;
		int maxGridZ = -INT_MAX;

		XMMATRIX objectWorld = CreateWorldMatrix(object);

		for (unsigned int meshIndex = 0; meshIndex < object.m_Asset->aiScene->mNumMeshes; meshIndex++)
		{
			const aiMesh* mesh = object.m_Asset->aiScene->mMeshes[meshIndex];
			if (!mesh) continue;

			XMMATRIX nodeToModel = XMLoadFloat4x4(&object.m_Asset->meshes[meshIndex].nodeToModel);
			XMMATRIX world = nodeToModel * objectWorld;

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				XMVECTOR localPos = XMVectorSet(
					mesh->mVertices[v].x,
					mesh->mVertices[v].y,
					mesh->mVertices[v].z,
					1.0f
				);

				XMVECTOR worldPos = XMVector3TransformCoord(localPos, world);

				float wx = XMVectorGetX(worldPos);
				float wy = XMVectorGetY(worldPos);
				float wz = XMVectorGetZ(worldPos);

				int gx = static_cast<int>(std::round(wx / PLAYFIELD_GRID_SIZE));
				int gz = static_cast<int>(std::round(wz / PLAYFIELD_GRID_SIZE));

				long long key = MakeGridKey(gx, gz);
				heightMap[key].y = wy;
				heightMap[key].exists = true;

				minGridX = (std::min)(minGridX, gx);
				maxGridX = (std::max)(maxGridX, gx);
				minGridZ = (std::min)(minGridZ, gz);
				maxGridZ = (std::max)(maxGridZ, gz);
			}
		}

		if (heightMap.empty()) return;

		for (int z = minGridZ; z < maxGridZ; z++)
		{
			for (int x = minGridX; x < maxGridX; x++)
			{
				HeightPoint p00 = heightMap[MakeGridKey(x,     z)];
				HeightPoint p10 = heightMap[MakeGridKey(x + 1, z)];
				HeightPoint p01 = heightMap[MakeGridKey(x,     z + 1)];
				HeightPoint p11 = heightMap[MakeGridKey(x + 1, z + 1)];

				if (!p00.exists || !p10.exists || !p01.exists || !p11.exists)
				{
					continue;
				}

				float topY = (p00.y + p10.y + p01.y + p11.y) / 4;

				AABB aabb{};
				aabb.min.x = x * PLAYFIELD_GRID_SIZE;
				aabb.max.x = (x + 1) * PLAYFIELD_GRID_SIZE;
				aabb.min.z = z * PLAYFIELD_GRID_SIZE;
				aabb.max.z = (z + 1) * PLAYFIELD_GRID_SIZE;
				aabb.max.y = topY;
				aabb.min.y = topY - PLAYFIELD_COLLIDER_THICKNESS;

				CollisionSystem::AddCollidersAABB(aabb);
			}
		}
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

		// Now using raycast
		/*
		if (name == "playField")
		{
			AddPlayFieldGridAABBs(g_EnvironmentObjects.back());
		}
		*/
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

