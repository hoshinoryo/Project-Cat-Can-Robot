/*==============================================================================

   ÉvÉåÉCÉÑÅ[êßå‰ [player.cpp]
														 Author : Gu Anyi
														 Date   : 2025/12/17

--------------------------------------------------------------------------------

==============================================================================*/

#include "Player.h"
#include "model_asset.h"
#include "model_renderer.h"
//#include "key_logger.h"
#include "Input.h"
#include "light.h"
#include "collision.h"
#include "Environment_Objects.h"
#include "Item.h"

#include <DirectXMath.h>
#include <cmath>


using namespace DirectX;

static constexpr float AABB_HALF_W = 1.0f;
static constexpr float AABB_HALF_D = 1.0;
static constexpr float AABB_HEIGHT = 6.0f;

static constexpr float KILL_Y = -50.0f;


Player::Player()
	//: m_State(AnimState::None)
{
}

Player::~Player()
{
	Finalize();
}

void Player::Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front)
{
	m_Position = position;
	m_Velocity = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&m_Front, XMVector3Normalize(XMLoadFloat3(&front)));
	m_IsJump = false;

	// Load player asset
	m_Asset = ModelAsset_Load("Asset/Character/Player.FBX", true, 0.8f);

	UpdateAABB();

	/*
	// Animation player
	m_AnimPlayer = new AnimationPlayer();

	// Load animation clip
	AnimationClip* idleClip = Animation_LoadFromFile("resources/Animation/Idle.fbx", m_Asset, true);
	int idleId = AnimationManager::Instance().RegisterClip(idleClip);
	m_ClipIdle = AnimationManager::Instance().GetClipById(idleId);

	m_ClipRun = nullptr;

	AnimationClip* jumpClip = Animation_LoadFromFile("resources/Animation/Jump.fbx", m_Asset, true);
	int jumpId = AnimationManager::Instance().RegisterClip(jumpClip);
	m_ClipJump = AnimationManager::Instance().GetClipById(jumpId);

	// Initialize state
	ChangeState(AnimState::Idle);
	*/
}

void Player::Finalize()
{
	if (m_Asset)
	{
		ModelAsset_Release(m_Asset);
		m_Asset = nullptr;
	}

	/*
	m_ClipIdle = nullptr;
	m_ClipWalk = nullptr;

	delete m_AnimPlayer;
	m_AnimPlayer = nullptr;
	*/
}

void Player::Update(double elapsed_time, const XMFLOAT3& cameraFront)
{
	UpdateMovement(elapsed_time, cameraFront);
	UpdatePhysics(elapsed_time);
	//UpdateState();
	//UpdateAnimation(elapsed_time);
}

/*
void Player::UpdateAnimationOnly(double elapsed_time)
{
	UpdateAnimation(elapsed_time);
}
*/

void Player::Draw(const XMFLOAT3& cameraPosition)
{
	if (!m_Asset) return;

	XMVECTOR pos = XMLoadFloat3(&m_Position);
	XMVECTOR front = XMLoadFloat3(&m_Front);

	if (XMVector3Equal(front, XMVectorZero()))
	{
		front = XMVectorSet(0, 0, 1, 0);
	}

	front = XMVectorSetY(front, 0.0f);
	front = XMVector3Normalize(front);

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	// LookTo
	XMMATRIX rot = XMMatrixInverse(nullptr, XMMatrixLookToLH(XMVectorZero(), front, up));

	XMMATRIX modelFix = XMMatrixRotationY(XM_PI);

	XMMATRIX trans = XMMatrixTranslationFromVector(pos);

	XMMATRIX world = modelFix * rot * trans;

	/*
	if (m_AnimPlayer)
	{
		Animation_UpdateSkinningCB(*m_AnimPlayer);
	}
	*/

	for (uint32_t mi = 0; mi < (uint32_t)m_Asset->meshes.size(); ++mi)
	{
		XMMATRIX nodeToModel = XMLoadFloat4x4(&m_Asset->meshes[mi].nodeToModel);
		XMMATRIX finalWorld = nodeToModel * world;

		ModelRenderer_Draw(m_Asset, mi, finalWorld, cameraPosition);
	}

	/*
	if (DebugDraw_Allow(DebugDrawCategory::Collision))
	{
		Collision_DebugDraw(m_WorldAABB, { 0.0f, 0.0f, 1.0f, 1.0f });
	}
	*/
}

void Player::DrawDepth()
{
	if (!m_Asset) return;

	XMVECTOR pos = XMLoadFloat3(&m_Position);
	XMVECTOR front = XMLoadFloat3(&m_Front);

	if (XMVector3Equal(front, XMVectorZero()))
	{
		front = XMVectorSet(0, 0, 1, 0);
	}

	front = XMVectorSetY(front, 0.0f);
	front = XMVector3Normalize(front);

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX rot = XMMatrixInverse(nullptr, XMMatrixLookToLH(XMVectorZero(), front, up));
	XMMATRIX modelFix = XMMatrixRotationY(XM_PI);
	XMMATRIX trans = XMMatrixTranslationFromVector(pos);
	XMMATRIX world = modelFix * rot * trans;

	/*
	if (m_AnimPlayer)
	{
		Animation_UpdateSkinningCB(*m_AnimPlayer);
	}
	*/

	for (uint32_t mi = 0; mi < (uint32_t)m_Asset->meshes.size(); ++mi)
	{
		XMMATRIX nodeToModel = XMLoadFloat4x4(&m_Asset->meshes[mi].nodeToModel);
		XMMATRIX finalWorld = nodeToModel * world;

		ModelRenderer_DrawDepth(m_Asset, mi, finalWorld);
	}
}

/*
void Player::SetState(AnimState state)
{
	ChangeState(state);
}
*/

// Movement
void Player::UpdateMovement(double elapsed_time, const XMFLOAT3& cameraFront)
{
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	// Camera forward -> XZ plane
	XMVECTOR front = XMLoadFloat3(&cameraFront);
	front = XMVectorSetY(front, 0.0f);

	if (XMVector3Equal(front, XMVectorZero()))
	{
		front = XMVectorSet(0, 0, 1, 0);
	}
	front = XMVector3Normalize(front);

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, front));

	float inputX = 0.0f;
	float inputZ = 0.0f;

	// Moving
	if (GetKeyPress('W')) inputZ += 1.0f;
	if (GetKeyPress('S')) inputZ -= 1.0f;
	if (GetKeyPress('A')) inputX -= 1.0f;
	if (GetKeyPress('D')) inputX += 1.0f;

	XMVECTOR moveDir = front * inputZ + right * inputX;

	if (!XMVector3Equal(moveDir, XMVectorZero()))
	{
		moveDir = XMVector3Normalize(moveDir);

		XMVECTOR pos = XMLoadFloat3(&m_Position);
		pos += moveDir * m_MoveSpeed * (float)elapsed_time;

		XMStoreFloat3(&m_Position, pos);
		XMStoreFloat3(&m_Front, moveDir);
	}

	XMStoreFloat3(&m_Velocity, velocity);
}

// Gravity and collision simulation
void Player::UpdatePhysics(double elapsed_time)
{
	const float dt = static_cast<float>(elapsed_time);

	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	// gravity
	float velocityY = XMVectorGetY(velocity);
	velocityY -= m_Gravity * dt;
	velocity = XMVectorSetY(velocity, velocityY);

	position += velocity * dt;

	XMFLOAT3 nextPosition{};
	XMStoreFloat3(&nextPosition, position);

	constexpr float MOVE_RADIUS = 50.0f;
	constexpr float MOVE_RADIUS_SQ = MOVE_RADIUS * MOVE_RADIUS;

	auto ClampToMoveRange = [](XMFLOAT3& value)
		{
			const float distanceSq = value.x * value.x + value.z * value.z;

			if (distanceSq > MOVE_RADIUS_SQ)
			{
				const float scale = MOVE_RADIUS / std::sqrt(distanceSq);

				value.x *= scale;
				value.z *= scale;
			}
		};

	ClampToMoveRange(nextPosition);

	m_Position = nextPosition;
	UpdateAABB();

	CollisionSystem::ResolveAgainstSceneXZ(m_AABB, m_Position, 4);

	ClampToMoveRange(m_Position);
	UpdateAABB();

	float groundY = 0.0f;

	const bool hasPlayField =
		EnvironmentObjects::GetPlayFieldY(m_Position.x, m_Position.z, groundY);

	const bool isFalling = XMVectorGetY(velocity) <= 0.0f;

	if (hasPlayField && isFalling && m_Position.y <= groundY)
	{
		m_Position.y = groundY;
		velocity = XMVectorSetY(velocity, 0.0f);

		m_IsGround = true;
		m_IsFall = false;
		m_IsJump = false;
	}
	else
	{
		m_IsGround = false;
		m_IsFall = XMVectorGetY(velocity) < 0.0f;
	}

	XMStoreFloat3(&m_Velocity, velocity);
	UpdateAABB();
}

/*
void Player::UpdateAnimation(double elapsed_time)
{
	if (!m_AnimPlayer) return;

	m_AnimPlayer->Update(elapsed_time);
}
*/

void Player::UpdateAABB()
{
	m_AABB.min = {
		m_Position.x - m_AABBHalfW,
		m_Position.y,
		m_Position.z - m_AABBHalfD
	};
	m_AABB.max = {
		m_Position.x + m_AABBHalfW,
		m_Position.y + m_AABBHeight,
		m_Position.z + m_AABBHalfD
	};
}

/*
void Player::UpdateState()
{
	if (!m_IsGround)
	{
		// è„Ç…å¸Ç≠
		if (m_Velocity.y > 0.1f)
		{
			ChangeState(AnimState::Jump);
			return;
		}
		// â∫Ç…ç~ÇËÇÈ
		if (m_IsFall)
		{
			ChangeState(AnimState::Fall);
			return;
		}

		ChangeState(AnimState::Jump);
		return;
	}

	if (m_IsMoving)
	{
		ChangeState(AnimState::Walk);
	}
	else
	{
		ChangeState(AnimState::Idle);
	}
}

void Player::CheckFallState(bool wasGround)
{
	if (m_IsGround) // when tough the ground
	{
		m_FallDistance = 0.0f;
		m_IsFall = false;
		return;
	}

	if (wasGround)
	{
		m_FallStartY = m_Position.y;
		m_FallDistance = 0.0f;
	}

	if (m_Velocity.y <= m_FallSpeedThredhold)
	{
		m_FallDistance = std::max(0.0f, m_FallStartY - m_Position.y);

		if (!m_IsFall && m_FallDistance >= m_FallAnimThredhold)
		{
			m_IsFall = true;
		}
	}
}

void Player::ChangeState(AnimState newState)
{
	if (!m_AnimPlayer || !m_Asset) return;
	if (m_State == newState) return;

	m_State = newState;

	switch (m_State)
	{
	case AnimState::Idle:
		if (m_ClipIdle)
		{
			m_AnimPlayer->Play(m_ClipIdle, m_Asset, true, 0.0);
		}
		break;

	case AnimState::Walk:
		if (m_ClipWalk)
		{
			m_AnimPlayer->Play(m_ClipWalk, m_Asset, true, 0.0);
		}
		break;

	case AnimState::Run:
		if (m_ClipRun)
		{
			m_AnimPlayer->Play(m_ClipRun, m_Asset, true, 0.0);
		}
		break;

	case AnimState::Jump:
		if (m_ClipJump)
		{
			m_AnimPlayer->Play(m_ClipJump, m_Asset, false, 0.0);
		}
		else if (m_ClipWalk)
		{
			m_AnimPlayer->Play(m_ClipWalk, m_Asset, true, 0.0);
		}
		break;

	case AnimState::Fall:
		if (m_ClipFall)
		{
			m_AnimPlayer->Play(m_ClipFall, m_Asset, true, 0.0);
		}
		break;

	default:
		break;
	}
}

// detect player foot and the ground
bool Player::CheckGroundProbe(float probe) const
{
	AABB probeBox = m_WorldAABB;
	probeBox.max.y = probeBox.min.y + probe;
	probeBox.min.y -= probe;

	for (const AABB& s : CollisionSystem::AllColliders())
	{
		if (Collision_IsOverlapAABB(probeBox, s))
			return true;
	}

	return false;
}
*/
