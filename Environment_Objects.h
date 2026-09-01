#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>
#include <vector>

#include "Collision.h"

struct ModelAsset;
//struct AABB;

namespace EnvironmentObjects
{
	struct StaticMeshCache
	{
		DirectX::XMFLOAT4X4 world{};
		AABB worldAABB{};
	};

	class EnvironmentObject
	{
	public:

		EnvironmentObject() = default;
		virtual ~EnvironmentObject() = default;

		virtual void Update(double elapsed_time);
		virtual void Draw(const DirectX::XMFLOAT3& cameraPosition, const DirectX::BoundingFrustum& cameraFrustum);
		virtual AABB GetAABB() const;

		void BuildStaticCache();

	public:

		std::string m_ObjectTag;
		ModelAsset* m_Asset{};

		DirectX::XMFLOAT3 m_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_Rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		std::vector<StaticMeshCache> m_StaticMeshes;
	};

	void Initialize();
	void Finalize();
	void Update(double elapsed_time);

	void AddObject(
		const std::string& name,
		ModelAsset* asset,
		const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)
	);
	
	void Draw(
		const DirectX::XMFLOAT3& cameraPosition,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

	// Playfield raycasting
	bool GetPlayFieldY(float x, float z, float& outY);
}
