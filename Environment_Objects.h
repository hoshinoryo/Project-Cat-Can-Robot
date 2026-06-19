#pragma once

#include <DirectXMath.h>
#include <string>

struct ModelAsset;
struct AABB;

namespace EnvironmentObjects
{
	class EnvironmentObject
	{
	public:

		EnvironmentObject() = default;
		virtual ~EnvironmentObject() = default;

		virtual void Update(double elapsed_time);
		virtual void Draw(const DirectX::XMFLOAT3& cameraPosition);
		virtual AABB GetAABB() const;

	public:

		std::string m_Name;
		ModelAsset* m_Asset{};

		DirectX::XMFLOAT3 m_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_Rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
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
	
	void Draw(const DirectX::XMFLOAT3& cameraPosition);

	// Playfield raycasting
	bool GetPlayFieldY(float x, float z, float& outY);
}
