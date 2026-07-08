/*==============================================================================

   ÉAÉCÉeÉÄê∂ê¨Ç∑ÇÈ [Item_Spawner.h]
														 Author : Gu Anyi
														 Date   : 2026/06/17

--------------------------------------------------------------------------------

==============================================================================*/

#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>

class Item;
class Player;
struct ModelAsset;

class ItemSpawner
{
public:

	void Initialize();
	void Finalize();

	void Update(double elapsed_time, Player& player);
	void Draw(const DirectX::XMFLOAT3& cameraPosition);

	//size_t GetItemCount() const;

	int GetCollectedItemCount() const;

private:

	void SpawnItem();
	void CheckCollisionWithPlayer(Player& player);

private:

	ModelAsset* m_ItemAsset = nullptr;

	std::vector<std::shared_ptr<Item>> m_Items;

	int m_CollectedItemCount = 0;

	float m_SpawnTimer = 0.0f;
	float m_SpawnInterval = 0.0f;

	// Spawner Range
	float m_MinX = -30.0f;
	float m_MaxX = 30.0f;
	float m_MinZ = -30.0f;
	float m_MaxZ = 30.0f;
	float m_SpawnY = 40.0f;
};