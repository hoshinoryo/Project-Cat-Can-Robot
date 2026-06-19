#pragma once

#include "Environment_Objects.h"
#include "Collision.h"

class Item : public EnvironmentObjects::EnvironmentObject
{
public:

	Item();

	void Update(double elapsed_time) override;
	void UpdatePhysics(double elapsed_time);

	DirectX::XMFLOAT3 GetPosition() const;
	AABB GetAABB() const override;

	bool IsDead() const;

	void SetVelocity(const DirectX::XMFLOAT3& velocity);

private:

	DirectX::XMFLOAT3 m_Velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_Gravity = -9.8f;
	float m_DeadY = -5.0f;
	float m_AABBHalfSize = 0.5f;
	//AABB m_LocalAABB;
};