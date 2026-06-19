#include "Item.h"

using namespace DirectX;

Item::Item()
{
    m_Name = "Item";

    m_Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Item::Update(double elapsed_time)
{
    UpdatePhysics(elapsed_time);
}

void Item::UpdatePhysics(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time) / 1000.0f;

    m_Velocity.y += m_Gravity * dt;

    m_Position.x += m_Velocity.x * dt;
    m_Position.y += m_Velocity.y * dt;
    m_Position.z += m_Velocity.z * dt;
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
    return m_Position.y <= m_DeadY;
}

void Item::SetVelocity(const DirectX::XMFLOAT3& velocity)
{
    m_Velocity = velocity;
}
