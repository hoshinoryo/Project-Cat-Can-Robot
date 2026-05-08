/*==============================================================================

   Player camera control [Camera.h]
														 Author : Gu Anyi
														 Date   : 2026/05/08
--------------------------------------------------------------------------------

==============================================================================*/

#include <DirectXMath.h>
#include <d3d11.h>


class PlayerCamera
{
public:

	bool Initialize();
	void Finalize();

	void SetFollowTarget(const DirectX::XMFLOAT3* playerPos);

	void SetActive(bool v) { m_Active = v; }
	bool IsActive() const { return m_Active; }

	void Update(double elapsed_time);

	const DirectX::XMFLOAT4X4& GetView() const { return m_View; }
	const DirectX::XMFLOAT4X4& GetProj() const { return m_Proj; }
	const DirectX::XMFLOAT4X4& GetInvView() const { return m_invView; }
	const DirectX::XMFLOAT4X4& GetInvProj() const { return m_invProj; }
	const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
	const DirectX::XMFLOAT3& GetFront() const { return m_Front; }

	float GetYaw() const { return m_Yaw; }

private:

	//void UpdateInput(double elapsed_time);
	void UpdateMatrices();

private:

	bool m_Active = false;

	const DirectX::XMFLOAT3* m_pPlayerPos = nullptr;

	float m_Yaw   = DirectX::XMConvertToRadians(180.0f);
	float m_Pitch = DirectX::XMConvertToRadians(20.0f);
	float m_CameraFov = 60.0f;

	float m_Distance = 6.0f;
	float m_Height = 5.0f;
	float m_LookHeight = 1.5f;

	float m_MouseSensitivity = 0.003f;

	DirectX::XMFLOAT3 m_Position{};
	DirectX::XMFLOAT3 m_Front{};

	DirectX::XMFLOAT4X4 m_View{};
	DirectX::XMFLOAT4X4 m_Proj{};
	DirectX::XMFLOAT4X4 m_invView{};
	DirectX::XMFLOAT4X4 m_invProj{};

	ID3D11Buffer* m_pVSConstantBufferView = nullptr;
	ID3D11Buffer* m_pVSConstantBufferProj = nullptr;

	int m_LastMouseX = 0;
	int m_LastMouseY = 0;

};


