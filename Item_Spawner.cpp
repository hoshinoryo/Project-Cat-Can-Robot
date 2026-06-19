#include "Item_Spawner.h"
#include "Item.h"
#include "model_asset.h"
//#include "Environment_Objects.h"

#include <random>

using namespace DirectX;

static std::mt19937 g_Random(std::random_device{}());

void ItemSpawner::Initialize()
{
    m_Items.clear();

    m_SpawnTimer = 0.0f;
    m_SpawnInterval = 200.0f;

    m_ItemAsset = ModelAsset_Load("Asset/Environment/Item/Item_Fish.fbx");

    SpawnItem();
}

void ItemSpawner::Finalize()
{
    m_Items.clear();

    m_ItemAsset = nullptr;
}

void ItemSpawner::Update(double elapsed_time)
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

void ItemSpawner::Draw(const XMFLOAT3& cameraPosition)
{
    for (std::shared_ptr<Item>& item : m_Items)
    {
        if (!item) continue;

        item->Draw(cameraPosition);
    }
}

size_t ItemSpawner::GetItemCount() const
{
    return m_Items.size();
}

void ItemSpawner::SpawnItem()
{
    if (!m_ItemAsset) return;

    std::uniform_real_distribution<float> distX(m_MinX, m_MaxX);
    std::uniform_real_distribution<float> distZ(m_MinZ, m_MaxZ);

    std::shared_ptr<Item> item = std::make_shared<Item>();

    item->m_Asset = m_ItemAsset;
    item->m_Position =
    {
        distX(g_Random),
        m_SpawnY,
        distZ(g_Random)
    };
    item->m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    item->m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

    m_Items.push_back(item);
}
