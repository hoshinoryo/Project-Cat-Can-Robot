#pragma once

#include <DirectXMath.h>
#include <string>

struct ModelAsset;

namespace EnvironmentObjects
{
	void Initialize();
	void Finalize();

	void AddObject(
		const std::string& name,
		ModelAsset* asset,
		const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)
	);
	void Draw(const DirectX::XMFLOAT3& cameraPosition);
}
