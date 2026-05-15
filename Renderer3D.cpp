#include <io.h>
#include <cstdio>
#include <assert.h>

#include "Renderer3D.h"
#include "Renderer_Manager.h"
#include "d3d11_state_guard_util.h"


using namespace DirectX;

static ID3D11Buffer* g_WorldBuffer3D{};
static ID3D11Buffer* g_ViewBuffer3D{};
static ID3D11Buffer* g_ProjectionBuffer3D{};

// for 3D
static ID3D11Buffer* g_DiffuseBuffer3D{};
static ID3D11Buffer* g_AmbientBuffer3D{};
static ID3D11Buffer* g_DirectionalLightBuffer3D{};
static ID3D11Buffer* g_SpecularBuffer3D{};
static ID3D11Buffer* g_PointLightBuffer3D{};
static ID3D11Buffer* g_ShadowBuffer3D{};

// depth state
static ID3D11DepthStencilState* g_DepthStateEnable3D{};  // for 3D depth state enable
static ID3D11DepthStencilState* g_DepthStateDisable3D{}; // for 2D depth state disable
static ID3D11DepthStencilState* g_DepthStateSkydome{};

static ID3D11BlendState* g_BlendStateTransparent3D{};
static ID3D11BlendState* g_BlendStateAdd3D{};

// rasterizer state
static ID3D11RasterizerState* g_RasterizerState3D{};
static ID3D11RasterizerState* g_RasterizerStateSkydome{};

static ID3D11SamplerState* g_SamplerState3D{};

// D3D device and device context
static ID3D11Device* g_Device{};
static ID3D11DeviceContext* g_DeviceContext{};

template<class T>
static void SafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

void Renderer3D_Initialize()
{
	g_Device = RendererManager_GetDevice();
	g_DeviceContext = RendererManager_GetDeviceContext();

	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	g_Device->CreateRasterizerState(&rasterizerDesc, &g_RasterizerState3D);

	// for skydome
	D3D11_RASTERIZER_DESC skyRasterizerDesc{};
	skyRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	skyRasterizerDesc.CullMode = D3D11_CULL_BACK;
	skyRasterizerDesc.DepthClipEnable = TRUE;
	skyRasterizerDesc.MultisampleEnable = FALSE;
	g_Device->CreateRasterizerState(&skyRasterizerDesc, &g_RasterizerStateSkydome);

	// ブレンドステート設定
	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateTransparent3D);

	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateAdd3D);

	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthDesc{};

	//深度無効ステート
	depthDesc.DepthEnable = FALSE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = FALSE;
	g_Device->CreateDepthStencilState(&depthDesc, &g_DepthStateDisable3D);//深度無効ステート

	//深度有効ステート
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	g_Device->CreateDepthStencilState(&depthDesc, &g_DepthStateEnable3D);

	D3D11_DEPTH_STENCIL_DESC skyDepthDesc{};
	skyDepthDesc.DepthEnable = TRUE;
	skyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	skyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	skyDepthDesc.StencilEnable = FALSE;
	g_Device->CreateDepthStencilState(&skyDepthDesc, &g_DepthStateSkydome);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Device->CreateSamplerState(&samplerDesc, &g_SamplerState3D);
	g_DeviceContext->PSSetSamplers(0, 1, &g_SamplerState3D);

	// --------------- 定数バッファ生成 ---------------------
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_WorldBuffer3D);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ViewBuffer3D);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ProjectionBuffer3D);

	bufferDesc.ByteWidth = sizeof(PS_DIFFUSE);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_DiffuseBuffer3D);

	bufferDesc.ByteWidth = sizeof(PS_AMBIENT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_AmbientBuffer3D);

	bufferDesc.ByteWidth = sizeof(PS_DIRECTIONAL_LIGHT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_DirectionalLightBuffer3D);

	bufferDesc.ByteWidth = sizeof(PS_SPECULAR);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_SpecularBuffer3D);

	bufferDesc.ByteWidth = sizeof(PS_POINT_LIGHT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_PointLightBuffer3D);

	bufferDesc.ByteWidth = sizeof(SHADOW_CONSTANT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ShadowBuffer3D);

	Renderer3D_SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Renderer3D_SetAmbientColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
	Renderer3D_SetDirectionalLight(
		XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	Renderer3D_SetSpecular(
		XMFLOAT3(0.0f, 2.0f, -5.0f),
		32.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);

	PS_POINT_LIGHT pointLight{};
	pointLight.PointLightCount = 0;
	Renderer3D_SetPointLight(pointLight);

	SHADOW_CONSTANT shadow{};
	XMStoreFloat4x4(&shadow.LightViewProj, XMMatrixTranspose(XMMatrixIdentity()));
	shadow.ShadowBias = 0.001f;
	shadow.ShadowStrength = 0.0f;
	shadow.ShadowTexelSize = XMFLOAT2(1.0f / 1024.0f, 1.0f / 1024.0f);
	Renderer3D_SetShadowConstant(shadow);

	//Renderer3D_Begin();
}

void Renderer3D_Finalize()
{
	SafeRelease(g_ShadowBuffer3D);
	SafeRelease(g_PointLightBuffer3D);
	SafeRelease(g_SpecularBuffer3D);
	SafeRelease(g_DirectionalLightBuffer3D);
	SafeRelease(g_AmbientBuffer3D);
	SafeRelease(g_DiffuseBuffer3D);

	SafeRelease(g_ProjectionBuffer3D);
	SafeRelease(g_ViewBuffer3D);
	SafeRelease(g_WorldBuffer3D);

	SafeRelease(g_SamplerState3D);

	SafeRelease(g_RasterizerStateSkydome);
	SafeRelease(g_RasterizerState3D);

	SafeRelease(g_BlendStateAdd3D);
	SafeRelease(g_BlendStateTransparent3D);

	SafeRelease(g_DepthStateSkydome);
	SafeRelease(g_DepthStateDisable3D);
	SafeRelease(g_DepthStateEnable3D);
}

void Renderer3D_BeginSkydome()
{
	g_DeviceContext->OMSetDepthStencilState(g_DepthStateSkydome, 0);
	g_DeviceContext->RSSetState(g_RasterizerStateSkydome);
}

void Renderer3D_EndSkydome()
{
	g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable3D, 0);
	g_DeviceContext->RSSetState(g_RasterizerState3D);
}

void Renderer3D_Begin()
{
	Renderer3D_SetDepthEnable(true);
	Renderer3D_SetAlphaBlendTransparent();

	g_DeviceContext->RSSetState(g_RasterizerState3D);

	g_DeviceContext->PSSetConstantBuffers(0, 1, &g_DiffuseBuffer3D);
	g_DeviceContext->PSSetConstantBuffers(1, 1, &g_AmbientBuffer3D);
	g_DeviceContext->PSSetConstantBuffers(2, 1, &g_DirectionalLightBuffer3D);
	g_DeviceContext->PSSetConstantBuffers(3, 1, &g_SpecularBuffer3D);
	g_DeviceContext->PSSetConstantBuffers(4, 1, &g_PointLightBuffer3D);

	g_DeviceContext->VSSetConstantBuffers(5, 1, &g_ShadowBuffer3D);
	g_DeviceContext->PSSetConstantBuffers(5, 1, &g_ShadowBuffer3D);
}

void Renderer3D_SetWorldMatrix(XMMATRIX WorldMatrix)
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, XMMatrixTranspose(WorldMatrix));
	g_DeviceContext->UpdateSubresource(g_WorldBuffer3D, 0, nullptr, &world, 0, 0);
}

void Renderer3D_SetViewMatrix(XMMATRIX ViewMatrix)
{
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, XMMatrixTranspose(ViewMatrix));
	g_DeviceContext->UpdateSubresource(g_ViewBuffer3D, 0, NULL, &view, 0, 0);
}

void Renderer3D_SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	XMFLOAT4X4 proj;
	XMStoreFloat4x4(&proj, XMMatrixTranspose(ProjectionMatrix));
	g_DeviceContext->UpdateSubresource(g_ProjectionBuffer3D, 0, NULL, &proj, 0, 0);
}

void Renderer3D_SetDiffuseColor(const XMFLOAT4& Color)
{
	PS_DIFFUSE diffuse{};
	diffuse.DiffuseColor = Color;
	g_DeviceContext->UpdateSubresource(g_DiffuseBuffer3D, 0, NULL, &diffuse, 0, 0);
}

void Renderer3D_SetAmbientColor(const XMFLOAT4& Color)
{
	PS_AMBIENT ambient{};
	ambient.AmbientColor = Color;
	g_DeviceContext->UpdateSubresource(g_AmbientBuffer3D, 0, NULL, &ambient, 0, 0);
}

void Renderer3D_SetDirectionalLight(const XMFLOAT4& Direction, const XMFLOAT4& Color)
{
	PS_DIRECTIONAL_LIGHT light{};
	light.DirectionalWorldVector = Direction;
	light.DirectionalColor = Color;
	g_DeviceContext->UpdateSubresource(g_DirectionalLightBuffer3D, 0, NULL, &light, 0, 0);
}

void Renderer3D_SetSpecular(const XMFLOAT3& EyePosW, float Power, const XMFLOAT4& Color)
{
	PS_SPECULAR specular{};
	specular.EyePosW = EyePosW;
	specular.SpecularPower = Power;
	specular.SpecularColor = Color;
	g_DeviceContext->UpdateSubresource(g_SpecularBuffer3D, 0, NULL, &specular, 0, 0);
}

void Renderer3D_SetPointLight(const PS_POINT_LIGHT& PointLight)
{
	g_DeviceContext->UpdateSubresource(g_PointLightBuffer3D, 0, NULL, &PointLight, 0, 0);
}

void Renderer3D_SetShadowConstant(const SHADOW_CONSTANT& ShadowConstant)
{
	g_DeviceContext->UpdateSubresource(g_ShadowBuffer3D, 0, NULL, &ShadowConstant, 0, 0);
}

void Renderer3D_SetDepthEnable(bool Enable)
{
	if (Enable)
		g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable3D, NULL);
	else
		g_DeviceContext->OMSetDepthStencilState(g_DepthStateDisable3D, NULL);
}

void Renderer3D_SetAlphaBlendTransparent()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendStateTransparent3D, blendFactor, 0xffffffff);
}

void Renderer3D_SetAlphaBlendAdd()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendStateAdd3D, blendFactor, 0xffffffff);
}

void Renderer3D_CreateVS(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	g_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	g_Device->CreateInputLayout(layout,
		numElements,
		buffer,
		fsize,
		VertexLayout);

	delete[] buffer;
}

void Renderer3D_CreatePS(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	g_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}
