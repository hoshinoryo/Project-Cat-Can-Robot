#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>

class Item;
struct ModelAsset;

class ItemSpawner
{
public:

	void Initialize();
	void Finalize();

	void Update(double elapsed_time);
	void Draw(const DirectX::XMFLOAT3& cameraPosition);

	size_t GetItemCount() const;

private:

	void SpawnItem();

private:

	ModelAsset* m_ItemAsset = nullptr;

	std::vector<std::shared_ptr<Item>> m_Items;

	float m_SpawnTimer = 0.0f;
	float m_SpawnInterval = 0.0f;

	// Spawmer Range
	float m_MinX = -50.0f;
	float m_MaxX = 50.0f;
	float m_MinZ = -50.0f;
	float m_MaxZ = 50.0f;
	float m_SpawnY = 30.0f;
};