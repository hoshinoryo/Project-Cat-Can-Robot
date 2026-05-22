#include "model_shader.h"

#include <fstream>
#include <Windows.h>

#include "Renderer_Manager.h"

using namespace DirectX;


static constexpr const char* STATIC_VS_CSO = "Shader/3dLitStaticVS.cso";
static constexpr const char* SKINNED_VS_CSO = "Shader/3dLitSkinnedVS.cso";
static constexpr const char* LIT_PS_CSO = "Shader/3dLitPS.cso";

static constexpr const wchar_t* STATIC_VS_HLSL = L"Shader/3dLitStaticVS.hlsl";
static constexpr const wchar_t* SKINNED_VS_HLSL = L"Shader/3dLitSkinnedVS.hlsl";
static constexpr const wchar_t* LIT_PS_HLSL = L"Shader/3dLitPS.hlsl";


namespace
{
	struct SpecularData
	{
		XMFLOAT3 CameraPosition;
		float Power;
		XMFLOAT4 Color;
	};

	template<class T>
	void ReleaseCOM(T*& ptr)
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
}

bool ModelShader::LoadShaderFile(const char* fileName, unsigned char** data, size_t* size)
{
	if (!fileName || !data || !size)
	{
		return false;
	}

	*data = nullptr;
	*size = 0;

	std::ifstream file(fileName, std::ios::binary);

	if (!file)
	{
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n", "エラー", MB_OK);
		return false;
	}

	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize <= 0)
	{
		return false;
	}

	unsigned char* buffer = new unsigned char[fileSize];

	if (!file.read(reinterpret_cast<char*>(buffer), fileSize))
	{
		delete[] buffer;
		return false;
	}

	*data = buffer;
	*size = static_cast<size_t>(fileSize);

	return true;
}

bool ModelShader::Initialize()
{
	Finalize();

	m_Device = RendererManager_GetDevice();
	m_Context = RendererManager_GetDeviceContext();

	if (!m_Device || !m_Context)
	{
		MessageBox(nullptr, "Renderer_ManagerからDeviceまたはContextを取得できませんでした。", "ModelShader Error", MB_OK);
		return false;
	}

	HRESULT hr = S_OK;

	// ---- Vertex Shader ----
	unsigned char* vsData = nullptr;
	size_t vsSize = 0;

	if (!LoadShaderFile(STATIC_VS_CSO, &vsData, &vsSize))
	{
		MessageBox(nullptr, STATIC_VS_CSO, "Vertex Shaderの読み込みに失敗しました", MB_OK);
		return false;
	}

	hr = m_Device->CreateVertexShader(vsData, vsSize, nullptr, &m_VertexShader);

	if (FAILED(hr))
	{
		delete[] vsData;
		MessageBox(nullptr, "Vertex Shaderの作成に失敗しました。", "ModelShader Error", MB_OK);
		return false;
	}

	// 頂点レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_Device->CreateInputLayout(
		layout,
		ARRAYSIZE(layout),
		vsData,
		vsSize,
		&m_InputLayout
	);

	delete[] vsData;

	if (FAILED(hr))
	{
		MessageBox(nullptr, "InputLayoutの作成に失敗しました。", "ModelShader Error", MB_OK);
		return false;
	}

	// ---- Pixel Shader ----
	unsigned char* psData = nullptr;
	size_t psSize = 0;

	if (!LoadShaderFile(LIT_PS_CSO, &psData, &psSize))
	{
		MessageBox(nullptr, LIT_PS_CSO, "Pixel Shaderの読み込みに失敗しました", MB_OK);
		return false;
	}

	hr = m_Device->CreatePixelShader(psData, psSize, nullptr, &m_PixelShader);

	delete[] psData;

	if (FAILED(hr))
	{
		MessageBox(nullptr, "Pixel Shaderの作成に失敗しました。", "ModelShader Error", MB_OK);
		return false;
	}

	// ---- Constant Buffers ----
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// VS b0 : World Matrix
	bufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	hr = m_Device->CreateBuffer(&bufferDesc, nullptr, &m_WorldBuffer);

	if (FAILED(hr))
	{
		MessageBox(nullptr, "WorldBufferの作成に失敗しました。", "ModelShader Error", MB_OK);
		return false;
	}

	// PS b0 : Diffuse Color
	bufferDesc.ByteWidth = sizeof(XMFLOAT4);
	hr = m_Device->CreateBuffer(&bufferDesc, nullptr, &m_DiffuseBuffer);

	if (FAILED(hr))
	{
		MessageBox(nullptr, "ColorBufferの作成に失敗しました。", "ModelShader Error", MB_OK);
		return false;
	}

	// PS b3 : Specular Params
	bufferDesc.ByteWidth = sizeof(SpecularData);
	hr = m_Device->CreateBuffer(&bufferDesc, nullptr, &m_SpecularBuffer);

	if (FAILED(hr))
	{
		MessageBox(nullptr, "SpecularBufferの作成に失敗しました。", "ModelShader Error", MB_OK);
		return false;
	}

	return true;
}

void ModelShader::Finalize()
{
	SafeRelease();

	m_Device = nullptr;
	m_Context = nullptr;
}

void ModelShader::SafeRelease()
{
	ReleaseCOM(m_SpecularBuffer);
	ReleaseCOM(m_DiffuseBuffer);
	ReleaseCOM(m_WorldBuffer);

	ReleaseCOM(m_InputLayout);
	ReleaseCOM(m_PixelShader);
	ReleaseCOM(m_VertexShader);
}

void ModelShader::Begin()
{
	if (!m_Context) return;

	m_Context->VSSetShader(m_VertexShader, nullptr, 0);
	m_Context->PSSetShader(m_PixelShader, nullptr, 0);

	m_Context->IASetInputLayout(m_InputLayout);

	m_Context->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_Context->PSSetConstantBuffers(0, 1, &m_DiffuseBuffer);
	//m_Context->PSSetConstantBuffers(1, 1, &m_AmbientBuffer);
	//m_Context->PSSetConstantBuffers(2, 1, &m_DirectionalLightBuffer);
	m_Context->PSSetConstantBuffers(3, 1, &m_SpecularBuffer);
	//m_Context->PSSetConstantBuffers(4, 1, &m_PointLightBuffer);
	//m_Context->PSSetConstantBuffers(5, 1, &m_ShadowBuffer);
}

void ModelShader::BeginDepthOnly()
{
	if (!m_Context) return;

	m_Context->VSSetShader(m_VertexShader, nullptr, 0);
	m_Context->PSSetShader(nullptr, nullptr, 0);

	m_Context->IASetInputLayout(m_InputLayout);

	m_Context->VSSetConstantBuffers(0, 1, &m_WorldBuffer);
}

void ModelShader::SetWorldMatrix(const XMMATRIX& world)
{
	if (!m_Context || !m_WorldBuffer) return;

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world));

	m_Context->UpdateSubresource(m_WorldBuffer, 0, nullptr, &matrix, 0, 0);
}

void ModelShader::SetColor(const XMFLOAT4& color)
{
	if (!m_Context || !m_DiffuseBuffer) return;

	m_Context->UpdateSubresource(m_DiffuseBuffer, 0, nullptr, &color, 0, 0);
}

void ModelShader::SetSpecularParams(
	const XMFLOAT3& cameraPos,
	float power,
	const XMFLOAT4& color
)
{
	if (!m_Context || !m_SpecularBuffer) return;

	SpecularData data{};
	data.CameraPosition = cameraPos;
	data.Power = power;
	data.Color = color;

	m_Context->UpdateSubresource(m_SpecularBuffer, 0, nullptr, &data, 0, 0);
}