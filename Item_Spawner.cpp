/*==============================================================================

   ÉAÉCÉeÉÄê∂ê¨Ç∑ÇÈ [Item_Spawner.cpp]
                                                         Author : Gu Anyi
                                                         Date   : 2026/06/17

--------------------------------------------------------------------------------

==============================================================================*/

#include "Item_Spawner.h"
#include "Item.h"
#include "model_asset.h"
#include "Player.h"
#include "Collision.h"

#include <cstdlib>
#include <random>
#include <algorithm>
#include <DirectXCollision.h>

using namespace DirectX;

static std::mt19937 g_Random(std::random_device{}());

void ItemSpawner::Initialize()
{
    m_Items.clear();

    //m_CollectedItemCount = 0;
    m_Score = 0;

    m_SpawnTimer = 0.0f;
    m_SpawnInterval = 1.0f;

    SpawnItem();
}

void ItemSpawner::Finalize()
{
    m_Items.clear();

    //m_ItemAsset = nullptr;
}

void ItemSpawner::Update(double elapsed_time, Player& player)
{
    float dt = static_cast<float>(elapsed_time);

    m_SpawnTimer += dt;

    if (m_SpawnTimer >= m_SpawnInterval)
    {
        m_SpawnTimer = 0.0f;
        SpawnItem();
    }

    for (std::shared_ptr<Item>& item : m_Items)
    {
        if (!item) continue;

        item->Update(elapsed_time);
    }

    CheckCollisionWithPlayer(player);

    m_Items.erase(
        std::remove_if(
            m_Items.begin(),
            m_Items.end(),
            [](const std::shared_ptr<Item>& item)
            {
                return !item || item->IsDead();
            }
        ),
        m_Items.end()
    );
}

void ItemSpawner::Draw(
    const DirectX::XMFLOAT3& cameraPosition,
    const DirectX::XMFLOAT4X4& view,
    const DirectX::XMFLOAT4X4& projection
)
{
    BoundingFrustum viewFrustum;

    BoundingFrustum::CreateFromMatrix(viewFrustum, XMLoadFloat4x4(&projection));

    const XMMATRIX inverseView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));

    BoundingFrustum worldFrustum;
    viewFrustum.Transform(worldFrustum, inverseView);

    for (std::shared_ptr<Item>& item : m_Items)
    {
        if (!item || !item->IsVisible()) continue;

        const AABB itemAABB = item->GetAABB();

        BoundingBox itemBounds{};
        itemBounds.Center = itemAABB.GetCenter();
        itemBounds.Extents = itemAABB.GetHalf();

        if (!worldFrustum.Intersects(itemBounds)) continue;

        item->BuildStaticCache();
        item->Draw(cameraPosition,worldFrustum);
    }
}

void ItemSpawner::CheckCollisionWithPlayer(Player& player)
{
    const AABB& playerAABB = player.GetAABB();

    for (std::shared_ptr<Item>& item : m_Items)
    {
        if (item->IsDead()) continue;

        if (Collision_IsOverlapAABB(playerAABB, item->GetAABB()))
        {
            m_Score += item->GetScore();

            item->Kill();
            //m_CollectedItemCount++;
        }
    }
}

void ItemSpawner::SpawnItem()
{
    //if (!m_ItemAsset) return;

    std::uniform_real_distribution<float> distX(m_MinX, m_MaxX);
    std::uniform_real_distribution<float> distZ(m_MinZ, m_MaxZ);

    ItemTag tag = ItemTag::ITEM_TAG_MAX; // Random tag
    int randomValue = rand() % 100;
    if (randomValue < 60)
    {
        tag = ItemTag::BOMB;
    }
    else
    {
        tag = ItemTag::FISH;
    }
    std::shared_ptr<Item> item = std::make_shared<Item>(tag);

    item->m_Asset = ModelAsset_Load(GetItemAssetPath(item->GetTag()), true);
    item->Initialize({ distX(g_Random), m_SpawnY, distZ(g_Random) });

    m_Items.push_back(item);
}
