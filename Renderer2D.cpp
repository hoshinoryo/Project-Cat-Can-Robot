
#include <io.h>
#include <cstdio>
#include <assert.h>

#include "main.h"
#include "Renderer2D.h"
#include "Renderer_Manager.h"
#include "d3d11_state_guard_util.h"


static ID3D11Buffer* g_WorldBuffer2D{};
static ID3D11Buffer* g_ViewBuffer2D{};
static ID3D11Buffer* g_ProjectionBuffer2D{};

// for 2D
static ID3D11Buffer* g_MaterialBuffer2D{};
static ID3D11Buffer* g_LightBuffer2D{};
static ID3D11Buffer* g_TexCoordBuffer2D{};

// depth state
static ID3D11DepthStencilState* g_DepthStateEnable2D{};
static ID3D11DepthStencilState* g_DepthStateDisable2D{};

// blend state
static ID3D11BlendState* g_BlendStateTransparent2D{};
static ID3D11BlendState* g_BlendStateATC2D{};
static ID3D11BlendState* g_BlendStateAdd2D{};

static ID3D11RasterizerState* g_RasterizerState2D{};
static ID3D11SamplerState* g_SamplerState2D{};

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


void Renderer2D_Initialize()
{
	HRESULT hr = S_OK;

	g_Device = RendererManager_GetDevice();
	g_DeviceContext = RendererManager_GetDeviceContext();

	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;

	g_Device->CreateRasterizerState(&rasterizerDesc, &g_RasterizerState2D);
	g_DeviceContext->RSSetState(g_RasterizerState2D);

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
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateTransparent2D);

	blendDesc.AlphaToCoverageEnable = TRUE;
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateATC2D);

	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateAdd2D);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendStateTransparent2D, blendFactor, 0xffffffff);

	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};

	//深度無効ステート
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	g_Device->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateDisable2D);//深度無効ステート

	//深度有効ステート
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;
	g_Device->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateEnable2D);

	g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable2D, NULL);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState{};
	g_Device->CreateSamplerState(&samplerDesc, &samplerState);
	g_DeviceContext->PSSetSamplers(0, 1, &samplerState);

	// --------------- 2D定数バッファ生成 ---------------------
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_WorldBuffer2D);
	g_DeviceContext->VSSetConstantBuffers(0, 1, &g_WorldBuffer2D);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ViewBuffer2D);
	g_DeviceContext->VSSetConstantBuffers(1, 1, &g_ViewBuffer2D);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ProjectionBuffer2D);
	g_DeviceContext->VSSetConstantBuffers(2, 1, &g_ProjectionBuffer2D);

	bufferDesc.ByteWidth = sizeof(MATERIAL);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_MaterialBuffer2D);
	g_DeviceContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer2D);
	g_DeviceContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer2D);

	bufferDesc.ByteWidth = sizeof(LIGHT);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_LightBuffer2D);
	g_DeviceContext->VSSetConstantBuffers(4, 1, &g_LightBuffer2D);
	g_DeviceContext->PSSetConstantBuffers(4, 1, &g_LightBuffer2D);

	bufferDesc.ByteWidth = sizeof(TEXCOORD);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_TexCoordBuffer2D);
	g_DeviceContext->VSSetConstantBuffers(5, 1, &g_TexCoordBuffer2D);
	g_DeviceContext->PSSetConstantBuffers(5, 1, &g_TexCoordBuffer2D);

	// ライト初期化
	LIGHT light{};
	light.Enable = true;
	light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.Diffuse = XMFLOAT4(1.5f, 1.5f, 1.5f, 1.0f);
	Renderer2D_SetLight(light);

	// マテリアル初期化
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer2D_SetMaterial(material);

	// テクスチャ座標初期化
	TEXCOORD texcoord{};
	texcoord.Position = XMFLOAT2(0.0f, 0.0f);
	texcoord.Scale = XMFLOAT2(1.0f, 1.0f);
	Renderer2D_SetTexCoord(texcoord);

	//Renderer2D_Begin();
}

void Renderer2D_Finalize()
{
	g_WorldBuffer2D->Release();
	g_ViewBuffer2D->Release();
	g_ProjectionBuffer2D->Release();
	g_LightBuffer2D->Release();
	g_MaterialBuffer2D->Release();
}

void Renderer2D_Begin()
{
	Renderer2D_SetDepthEnable(false);
	Renderer2D_SetAlphaBlendTransparent();

	g_DeviceContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer2D);
	g_DeviceContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer2D);

	g_DeviceContext->VSSetConstantBuffers(4, 1, &g_LightBuffer2D);
	g_DeviceContext->PSSetConstantBuffers(4, 1, &g_LightBuffer2D);

	g_DeviceContext->VSSetConstantBuffers(5, 1, &g_TexCoordBuffer2D);
	g_DeviceContext->PSSetConstantBuffers(5, 1, &g_TexCoordBuffer2D);

	Renderer2D_SetWorldViewProjection();
}


void Renderer2D_SetATCEnable(bool Enable)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		g_DeviceContext->OMSetBlendState(g_BlendStateATC2D, blendFactor, 0xffffffff);
	else
		g_DeviceContext->OMSetBlendState(g_BlendStateTransparent2D, blendFactor, 0xffffffff);

}

void Renderer2D_SetWorldViewProjection()
{
	Renderer2D_SetWorldMatrix(XMMatrixIdentity());
	Renderer2D_SetViewMatrix(XMMatrixIdentity());

	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH(
		0.0f,
		static_cast<float>(RendererManager_GetBackBufferWidth()),
		static_cast<float>(RendererManager_GetBackBufferHeight()),
		0.0f,
		0.0f,
		1.0f
	);
	Renderer2D_SetProjectionMatrix(projection);
}


void Renderer2D_SetWorldMatrix(XMMATRIX WorldMatrix)
{
	XMFLOAT4X4 worldf;
	XMStoreFloat4x4(&worldf, XMMatrixTranspose(WorldMatrix));
	g_DeviceContext->UpdateSubresource(g_WorldBuffer2D, 0, NULL, &worldf, 0, 0);
}

void Renderer2D_SetViewMatrix(XMMATRIX ViewMatrix)
{
	XMFLOAT4X4 viewf;
	XMStoreFloat4x4(&viewf, XMMatrixTranspose(ViewMatrix));
	g_DeviceContext->UpdateSubresource(g_ViewBuffer2D, 0, NULL, &viewf, 0, 0);
}

void Renderer2D_SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	XMFLOAT4X4 projectionf;
	XMStoreFloat4x4(&projectionf, XMMatrixTranspose(ProjectionMatrix));
	g_DeviceContext->UpdateSubresource(g_ProjectionBuffer2D, 0, NULL, &projectionf, 0, 0);

}

void Renderer2D_SetMaterial(const MATERIAL& Material)
{
	g_DeviceContext->UpdateSubresource(g_MaterialBuffer2D, 0, NULL, &Material, 0, 0);
}

void Renderer2D_SetLight(const LIGHT& Light)
{
	g_DeviceContext->UpdateSubresource(g_LightBuffer2D, 0, NULL, &Light, 0, 0);
}

void Renderer2D_SetTexCoord(const TEXCOORD& TexCoord)
{
	g_DeviceContext->UpdateSubresource(g_TexCoordBuffer2D, 0, NULL, &TexCoord, 0, 0);
}


void Renderer2D_SetDepthEnable(bool Enable)
{
	if (Enable)
		g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable2D, NULL);
	else
		g_DeviceContext->OMSetDepthStencilState(g_DepthStateDisable2D, NULL);
}

void Renderer2D_SetAlphaBlendTransparent()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendStateTransparent2D, blendFactor, 0xffffffff);
}

void Renderer2D_SetAlphaBlendAdd()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendStateAdd2D, blendFactor, 0xffffffff);
}


void Renderer2D_CreateVS(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{
	FILE* file = nullptr;

	errno_t err = fopen_s(&file, FileName, "rb");
	assert(err == 0 && file);

	long int fsize = _filelength(_fileno(file));

	unsigned char* buffer = new unsigned char[fsize];

	fread(buffer, fsize, 1, file);
	fclose(file);

	g_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

void Renderer2D_CreatePS(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file = nullptr;

	errno_t err = fopen_s(&file, FileName, "rb");
	assert(err == 0 && file);

	long int fsize = _filelength(_fileno(file));

	unsigned char* buffer = new unsigned char[fsize];

	fread(buffer, fsize, 1, file);
	fclose(file);

	g_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}


