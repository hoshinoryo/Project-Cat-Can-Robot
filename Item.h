/*==============================================================================

   アイテム定義 [Item.h]
														 Author : Gu Anyi
														 Date   : 2026/06/17

--------------------------------------------------------------------------------

==============================================================================*/

#pragma once

#include "Environment_Objects.h"
#include "Collision.h"
#include "Item_Tag.h"

class Item : public EnvironmentObjects::EnvironmentObject
{
public:

	Item(ItemTag tag);

	void Initialize(
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }
	);
	void Update(double elapsed_time) override;

	DirectX::XMFLOAT3 GetPosition() const;
	AABB GetAABB() const override;
	ItemTag GetTag() const { return m_Tag; }
	int GetScore() const { return GetItemScore(m_Tag); }

	bool IsDead() const;
	void Kill();

	void SetVelocity(const DirectX::XMFLOAT3& velocity);

private:

	void UpdatePhysics(double elapsed_time);
	void UpdateEraseTimer(double elapsed_time);
	void Bounce(float playfieldY);

private:

	ItemTag m_Tag = ItemTag::ITEM_TAG_MAX;
	DirectX::XMFLOAT3 m_Velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_Gravity = -9.8f;
	float m_DeadY = -5.0f;
	float m_AABBHalfSize = 2.0f;

	bool m_IsGround = false;
	bool m_IsDead = false;
	
	float m_EraseTimer = 0.0f;
	float m_EraseTime = 3.0f;

	int m_BounceCount = 0;
	int m_MaxBounce = 2;
	float m_BouncePower = 5.0f;
};