/*==============================================================================

   Shader management for CPU [model_shader.h]
														 Author : Gu Anyi
														 Date   : 2026/05/21
--------------------------------------------------------------------------------

==============================================================================*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class ModelShader
{
private:

	ID3D11Device*        m_Device = nullptr;
	ID3D11DeviceContext* m_Context = nullptr;
	
	// d3d11 resources
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader*  m_PixelShader = nullptr;
	ID3D11InputLayout*  m_InputLayout = nullptr;
	
	// VS constant buffer
	ID3D11Buffer* m_WorldBuffer = nullptr;     // VS b0

	// PS constant buffer
	ID3D11Buffer* m_DiffuseBuffer = nullptr;
	ID3D11Buffer* m_AmbientBuffer = nullptr;
	ID3D11Buffer* m_DirectionalLightBuffer = nullptr;
	ID3D11Buffer* m_SpecularBuffer = nullptr;
	ID3D11Buffer* m_PointLightBuffer = nullptr;
	ID3D11Buffer* m_ShadowBuffer = nullptr;

private:

	bool LoadShaderFile(const char* fileName, unsigned char** data, size_t* size);
	void SafeRelease();

public:

	ModelShader() = default;
	~ModelShader() = default;

	bool Initialize();
	void Finalize();

	void Begin();
	void BeginDepthOnly();

	void SetWorldMatrix(const DirectX::XMMATRIX& world);

	void SetColor(const DirectX::XMFLOAT4& color);
	void SetSpecularParams(
		const DirectX::XMFLOAT3& cameraPos,
		float power,
		const DirectX::XMFLOAT4& color
	);

	ID3D11VertexShader* GetVertexShader() const { return m_VertexShader; }
	ID3D11PixelShader* GetPixelShader() const { return m_PixelShader; }
	ID3D11InputLayout* GetInputLayout() const { return m_InputLayout; }
};
