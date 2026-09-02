#include <vector>
#include <assert.h>
#include <cmath>
#include <cfloat>
#include <algorithm>

#include "Environment_Objects.h"
#include "model_asset.h"
#include "model_renderer.h"
#include "Collision.h"

using namespace DirectX;

namespace EnvironmentObjects
{	
	struct StaticTriangle
	{
		XMFLOAT3 p0{};
		XMFLOAT3 p1{};
		XMFLOAT3 p2{};

		float minX{};
		float maxX{};
		float minZ{};
		float maxZ{};
	};

	static std::vector<EnvironmentObject> g_EnvironmentObjects;
	static std::vector<StaticTriangle> g_PlayFieldTriangles; // cache

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

	static void CachePlayFieldTriangles(const EnvironmentObject& object)
	{
		if (!object.m_Asset) return;

		for (size_t meshIndex = 0; meshIndex < object.m_Asset->meshes.size(); meshIndex++)
		{
			const MeshAsset& mesh = object.m_Asset->meshes[meshIndex];

			if (mesh.Name != "SM_PlayField") continue;
			if (meshIndex >= object.m_StaticMeshes.size()) continue;

			const XMMATRIX world = XMLoadFloat4x4(&object.m_StaticMeshes[meshIndex].world);

			for (size_t i = 0; i + 2 < mesh.cpuIndices.size(); i += 3)
			{
				const uint32_t i0 = mesh.cpuIndices[i + 0];
				const uint32_t i1 = mesh.cpuIndices[i + 1];
				const uint32_t i2 = mesh.cpuIndices[i + 2];

				if (i0 >= mesh.cpuVertices.size() ||
					i1 >= mesh.cpuVertices.size() ||
					i2 >= mesh.cpuVertices.size())
				{
					continue;
				}

				StaticTriangle triangle{};

				XMStoreFloat3(&triangle.p0, XMVector3TransformCoord(XMLoadFloat3(&mesh.cpuVertices[i0].Position), world));
				XMStoreFloat3(&triangle.p1, XMVector3TransformCoord(XMLoadFloat3(&mesh.cpuVertices[i1].Position), world));
				XMStoreFloat3(&triangle.p2, XMVector3TransformCoord(XMLoadFloat3(&mesh.cpuVertices[i2].Position), world));

				triangle.minX = (std::min)(triangle.p0.x, (std::min)(triangle.p1.x, triangle.p2.x));
				triangle.maxX = (std::max)(triangle.p0.x, (std::max)(triangle.p1.x, triangle.p2.x));
				triangle.minZ = (std::min)(triangle.p0.z, (std::min)(triangle.p1.z, triangle.p2.z));
				triangle.maxZ = (std::max)(triangle.p0.z, (std::max)(triangle.p1.z, triangle.p2.z));

				g_PlayFieldTriangles.push_back(triangle);
			}
		}
	}

	void EnvironmentObject::Update(double elapsed_time)
	{
	}

	void EnvironmentObject::Draw(const XMFLOAT3& cameraPosition, const DirectX::BoundingFrustum& cameraFrustum)
	{
		if (!m_Asset) return;

		for (uint32_t meshIndex = 0; meshIndex < m_Asset->meshes.size(); meshIndex++)
		{
			if (meshIndex >= m_StaticMeshes.size()) continue;

			const MeshAsset& mesh = m_Asset->meshes[meshIndex];
			const StaticMeshCache& cache = m_StaticMeshes[meshIndex];

			BoundingBox meshBounds{};
			meshBounds.Center = cache.worldAABB.GetCenter();
			meshBounds.Extents = cache.worldAABB.GetHalf();

			if (!cameraFrustum.Intersects(meshBounds)) continue;
			const bool useAABBCollision = m_ObjectTag == "playField"
				&& mesh.Name != "SM_PlayField"
				&& mesh.Name != "SM_Fence"
				&& mesh.Name != "SM_Bush_TypeA"
				&& mesh.Name != "SM_Bush_TypeB";

			if (useAABBCollision)
			{
				CollisionSystem::AddCollidersAABB(cache.worldAABB);
			}

			const XMMATRIX finalWorld = XMLoadFloat4x4(&cache.world);

			ModelRenderer_Draw(m_Asset, meshIndex, finalWorld, cameraPosition);
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

	void EnvironmentObject::BuildStaticCache()
	{
		m_StaticMeshes.clear();

		if (!m_Asset) return;

		const XMMATRIX objectWorld = CreateWorldMatrix(*this);
		m_StaticMeshes.resize(m_Asset->meshes.size());

		for (size_t i = 0; i < m_Asset->meshes.size(); ++i)
		{
			const MeshAsset& mesh = m_Asset->meshes[i];
			const XMMATRIX nodeToModel = XMLoadFloat4x4(&mesh.nodeToModel);
			const XMMATRIX finalWorld = nodeToModel * objectWorld;

			StaticMeshCache& cache = m_StaticMeshes[i];

			XMStoreFloat4x4(&cache.world, finalWorld);
			cache.worldAABB = Collision_TransformAABB(mesh.localAABB, finalWorld);
		}
	}

	void Initialize()
	{
		// Clear
		g_EnvironmentObjects.clear();
		g_PlayFieldTriangles.clear();
		CollisionSystem::ClearColliders();

		// Import models
		ModelAsset* playField = ModelAsset_Load("Asset/Environment/PlayField.fbx", true);
		assert(playField);

		AddObject("playField", playField);
	}

	void Finalize()
	{
		g_PlayFieldTriangles.clear();
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
		object.m_ObjectTag = name;
		object.m_Asset = asset;
		object.m_Position = position;
		object.m_Rotation = rotation;
		object.m_Scale = scale;

		object.BuildStaticCache();

		if (name == "playField")
		{
			CachePlayFieldTriangles(object);
		}

		g_EnvironmentObjects.push_back(std::move(object));
	}

	void Draw(
		const DirectX::XMFLOAT3& cameraPosition,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	)
	{
		CollisionSystem::ClearColliders();

		BoundingFrustum viewFrustum;
		BoundingFrustum::CreateFromMatrix(viewFrustum, XMLoadFloat4x4(&projection));

		const XMMATRIX inverseView =XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));

		BoundingFrustum worldFrustum;
		viewFrustum.Transform(worldFrustum, inverseView);

		for (EnvironmentObject& object : g_EnvironmentObjects)
		{
			object.Draw(cameraPosition, worldFrustum);
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

		for (const StaticTriangle& triangle : g_PlayFieldTriangles)
		{
			if (x < triangle.minX || x > triangle.maxX ||
				z < triangle.minZ || z > triangle.maxZ)
			{
				continue;
			}

			float t = 0.0f;

			if (RayIntersectsTriangle(rayOrigin, rayDir,
				XMLoadFloat3(&triangle.p0),
				XMLoadFloat3(&triangle.p1),
				XMLoadFloat3(&triangle.p2), t)
				)
			{
				if (t < nearestT && t <= RAY_MAX_DISTANCE)
				{
					nearestT = t;
					hit = true;
				}
			}
		}

		if (!hit) return false;

		outY = RAY_START_Y - nearestT;
		return true;
	}
}

