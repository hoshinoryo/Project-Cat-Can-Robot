/*==============================================================================

   アイテム定義 [Item.cpp]
                                                         Author : Gu Anyi
                                                         Date   : 2026/06/17

--------------------------------------------------------------------------------

==============================================================================*/

#include "Item.h"

using namespace DirectX;

Item::Item(ItemTag tag)
    : m_Tag(tag)
{
    m_ObjectTag = "Item";
}

void Item::Initialize(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
    m_Position = position;
    m_Rotation = rotation;
    m_Scale = scale;
}

void Item::Update(double elapsed_time)
{
    if (!m_IsGround)
    {
        UpdatePhysics(elapsed_time);
    }
    else
    {
        UpdateEraseTimer(elapsed_time);
    }
}

void Item::UpdatePhysics(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time);

    m_Velocity.y += m_Gravity * dt; // gravity

    m_Position.x += m_Velocity.x * dt;
    m_Position.y += m_Velocity.y * dt;
    m_Position.z += m_Velocity.z * dt;

    // raycasting with playfield
    float playfieldY = 0.0f;

    // collision
    if (EnvironmentObjects::GetPlayFieldY(m_Position.x, m_Position.z, playfieldY))
    {
        float itemBottomY = m_Position.y - m_AABBHalfSize;

        if (itemBottomY <= playfieldY && m_Velocity.y <= 0.0f)
        {
            Bounce(playfieldY);
        }
    }
}

void Item::UpdateEraseTimer(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time);

    m_EraseTimer += dt;

    if (m_EraseTimer >= m_EraseTime)
    {
        m_IsDead = true;
    }
}

void Item::Bounce(float playfieldY)
{
    // position correction
    m_Position.y = playfieldY + m_AABBHalfSize;

    m_BounceCount++;

    // velocity correction
    if (m_BounceCount <= m_MaxBounce)
    {
        m_Velocity.y = m_BouncePower;
    }
    else
    {
        m_Velocity.y = 0.0f;
        m_IsGround = true;
        m_EraseTimer = 0.0f;
    }
}

XMFLOAT3 Item::GetPosition() const
{
    return m_Position;
}

AABB Item::GetAABB() const
{
    AABB aabb{};

    aabb.min = XMFLOAT3(
        m_Position.x - m_AABBHalfSize,
        m_Position.y - m_AABBHalfSize,
        m_Position.z - m_AABBHalfSize
    );
    aabb.max = XMFLOAT3(
        m_Position.x + m_AABBHalfSize,
        m_Position.y + m_AABBHalfSize,
        m_Position.z + m_AABBHalfSize
    );

    return aabb;
}

bool Item::IsDead() const
{
    return m_IsDead;
}

void Item::Kill()
{
    m_IsDead = true;
}

void Item::SetVelocity(const DirectX::XMFLOAT3& velocity)
{
    m_Velocity = velocity;
}
