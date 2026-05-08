
#include "main.h"
#include "renderer.h"
#include <io.h>


D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* g_Device{};
ID3D11DeviceContext* g_DeviceContext{};
IDXGISwapChain* g_SwapChain{};
ID3D11RenderTargetView* g_RenderTargetView{};
ID3D11DepthStencilView* g_DepthStencilView{};
//ID3D11Texture2D* g_DepthStencilBuffer{};

ID3D11Buffer* g_WorldBuffer{};
ID3D11Buffer* g_ViewBuffer{};
ID3D11Buffer* g_ProjectionBuffer{};

// for 2D
ID3D11Buffer* g_MaterialBuffer{};
ID3D11Buffer* g_LightBuffer{};
ID3D11Buffer* g_TexCoordBuffer{};

// for 3D
ID3D11Buffer* g_DiffuseBuffer{};
ID3D11Buffer* g_AmbientBuffer{};
ID3D11Buffer* g_DirectionalLightBuffer{};
ID3D11Buffer* g_SpecularBuffer{};
ID3D11Buffer* g_PointLightBuffer{};
ID3D11Buffer* g_ShadowBuffer{};

// depth state
ID3D11DepthStencilState* g_DepthStateEnable{};  // for 3D depth state enable
ID3D11DepthStencilState* g_DepthStateDisable{}; // for 2D depth state disable
ID3D11DepthStencilState* g_DepthStateSkydome{};

// blend state
ID3D11BlendState* g_BlendState{};
ID3D11BlendState* g_BlendStateATC{};
ID3D11BlendState* g_BlendStateAdd{};

// rasterizer state
ID3D11RasterizerState* g_RasterizerState{};
ID3D11RasterizerState* g_RasterizerStateSkydome{};

D3D11_TEXTURE2D_DESC g_BackBufferDesc{};
D3D11_VIEWPORT g_Viewport{};

ID3D11Device* GetDevice() { return g_Device; }
ID3D11DeviceContext* GetDeviceContext() { return g_DeviceContext; }

unsigned int GetBackBufferWidth()
{
	return g_BackBufferDesc.Width;
}

unsigned int GetBackBufferHeight()
{
	return g_BackBufferDesc.Height;
}

void InitRenderer()
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetWindow();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&g_SwapChain,
		&g_Device,
		&g_FeatureLevel,
		&g_DeviceContext);



	// レンダーターゲットビュー作成
	ID3D11Texture2D* renderTarget{};
	g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTarget);

	renderTarget->GetDesc(&g_BackBufferDesc);

	g_Device->CreateRenderTargetView(renderTarget, NULL, &g_RenderTargetView);
	renderTarget->Release();



	// デプスステンシルバッファ作成
	ID3D11Texture2D* depthStencile{};
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = g_BackBufferDesc.Width;
	textureDesc.Height = g_BackBufferDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	g_Device->CreateTexture2D(&textureDesc, NULL, &depthStencile);

	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	g_Device->CreateDepthStencilView(depthStencile, &depthStencilViewDesc, &g_DepthStencilView);
	//g_DepthStencilBuffer = depthStencile;

	depthStencile->Release();


	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);



	// ビューポート設定
	//D3D11_VIEWPORT viewport;
	g_Viewport.Width = static_cast<FLOAT>(g_BackBufferDesc.Width);
	g_Viewport.Height = static_cast<FLOAT>(g_BackBufferDesc.Height);
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;
	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_DeviceContext->RSSetViewports(1, &g_Viewport);



	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;

	g_Device->CreateRasterizerState(&rasterizerDesc, &g_RasterizerState);
	g_DeviceContext->RSSetState(g_RasterizerState);

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
	g_Device->CreateBlendState(&blendDesc, &g_BlendState);

	blendDesc.AlphaToCoverageEnable = TRUE;
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateATC);

	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_Device->CreateBlendState(&blendDesc, &g_BlendStateAdd);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendState, blendFactor, 0xffffffff);





	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};

	//深度無効ステート
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	g_Device->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateDisable);//深度無効ステート

	//深度有効ステート
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;
	g_Device->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateEnable);

	g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);


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

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_WorldBuffer);
	g_DeviceContext->VSSetConstantBuffers(0, 1, &g_WorldBuffer);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ViewBuffer);
	g_DeviceContext->VSSetConstantBuffers(1, 1, &g_ViewBuffer);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ProjectionBuffer);
	g_DeviceContext->VSSetConstantBuffers(2, 1, &g_ProjectionBuffer);


	bufferDesc.ByteWidth = sizeof(MATERIAL);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_MaterialBuffer);
	g_DeviceContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer);
	g_DeviceContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer);


	bufferDesc.ByteWidth = sizeof(LIGHT);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_LightBuffer);
	g_DeviceContext->VSSetConstantBuffers(4, 1, &g_LightBuffer);
	g_DeviceContext->PSSetConstantBuffers(4, 1, &g_LightBuffer);


	bufferDesc.ByteWidth = sizeof(TEXCOORD);

	g_Device->CreateBuffer(&bufferDesc, NULL, &g_TexCoordBuffer);
	g_DeviceContext->VSSetConstantBuffers(5, 1, &g_TexCoordBuffer);
	g_DeviceContext->PSSetConstantBuffers(5, 1, &g_TexCoordBuffer);


	// --------------- 3D定数バッファ生成 ---------------------
	bufferDesc.ByteWidth = sizeof(PS_DIFFUSE);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_DiffuseBuffer);

	bufferDesc.ByteWidth = sizeof(PS_AMBIENT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_AmbientBuffer);

	bufferDesc.ByteWidth = sizeof(PS_DIRECTIONAL_LIGHT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_DirectionalLightBuffer);

	bufferDesc.ByteWidth = sizeof(PS_SPECULAR);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_SpecularBuffer);

	bufferDesc.ByteWidth = sizeof(PS_POINT_LIGHT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_PointLightBuffer);

	bufferDesc.ByteWidth = sizeof(SHADOW_CONSTANT);
	g_Device->CreateBuffer(&bufferDesc, NULL, &g_ShadowBuffer);


	// ライト初期化
	LIGHT light{};
	light.Enable = true;
	light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.Diffuse = XMFLOAT4(1.5f, 1.5f, 1.5f, 1.0f);
	SetLight(light);



	// マテリアル初期化
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);



	// テクスチャ座標初期化
	TEXCOORD texcoord{};
	texcoord.Position = XMFLOAT2(0.0f, 0.0f);
	texcoord.Scale = XMFLOAT2(1.0f, 1.0f);
	SetTexCoord(texcoord);

	// 3D初期化
	SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetAmbientColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

	SetDirectionalLight(
		XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);

	SetSpecular(
		XMFLOAT3(0.0f, 2.0f, -5.0f),
		32.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	);

	PS_POINT_LIGHT pointLight{};
	pointLight.PointLightCount = 0;
	SetPointLight(pointLight);

	SHADOW_CONSTANT shadow{};
	XMStoreFloat4x4(&shadow.LightViewProj, XMMatrixTranspose(XMMatrixIdentity()));
	shadow.ShadowBias = 0.001f;
	shadow.ShadowStrength = 0.0f;
	shadow.ShadowTexelSize = XMFLOAT2(1.0f / 1024.0f, 1.0f / 1024.0f);
	SetShadowConstant(shadow);

	BeginRenderer2D();
}



void UninitRenderer()
{
	if (g_ShadowBuffer) g_ShadowBuffer->Release();
	if (g_PointLightBuffer) g_PointLightBuffer->Release();
	if (g_SpecularBuffer) g_SpecularBuffer->Release();
	if (g_DirectionalLightBuffer) g_DirectionalLightBuffer->Release();
	if (g_AmbientBuffer) g_AmbientBuffer->Release();
	if (g_DiffuseBuffer) g_DiffuseBuffer->Release();

	if (g_BlendStateAdd) g_BlendStateAdd->Release();
	if (g_RasterizerStateSkydome) g_RasterizerStateSkydome->Release();
	if (g_RasterizerState) g_RasterizerState->Release();
	if (g_DepthStateSkydome) g_DepthStateSkydome->Release();
	//if (g_DepthStencilBuffer) g_DepthStencilBuffer->Release();

	g_WorldBuffer->Release();
	g_ViewBuffer->Release();
	g_ProjectionBuffer->Release();
	g_LightBuffer->Release();
	g_MaterialBuffer->Release();


	g_DeviceContext->ClearState();
	g_RenderTargetView->Release();
	g_SwapChain->Release();
	g_DeviceContext->Release();
	g_Device->Release();

}



void BeginFrame()
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	g_DeviceContext->ClearRenderTargetView(g_RenderTargetView, clearColor);
	g_DeviceContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_DeviceContext->RSSetViewports(1, &g_Viewport);
}

void BeginRenderer2D()
{
	SetDepthEnable(false);
	SetAlphaBlendTransparent();

	g_DeviceContext->RSSetState(g_RasterizerState);

	g_DeviceContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer);
	g_DeviceContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer);

	g_DeviceContext->VSSetConstantBuffers(4, 1, &g_LightBuffer);
	g_DeviceContext->PSSetConstantBuffers(4, 1, &g_LightBuffer);

	g_DeviceContext->VSSetConstantBuffers(5, 1, &g_TexCoordBuffer);
	g_DeviceContext->PSSetConstantBuffers(5, 1, &g_TexCoordBuffer);

	SetWorldViewProjection2D();
}

void BeginRenderer3D()
{
	SetDepthEnable(true);
	SetAlphaBlendTransparent();

	g_DeviceContext->RSSetState(g_RasterizerState);

	g_DeviceContext->PSSetConstantBuffers(0, 1, &g_DiffuseBuffer);
	g_DeviceContext->PSSetConstantBuffers(1, 1, &g_AmbientBuffer);
	g_DeviceContext->PSSetConstantBuffers(2, 1, &g_DirectionalLightBuffer);
	g_DeviceContext->PSSetConstantBuffers(3, 1, &g_SpecularBuffer);
	g_DeviceContext->PSSetConstantBuffers(4, 1, &g_PointLightBuffer);

	g_DeviceContext->VSSetConstantBuffers(5, 1, &g_ShadowBuffer);
	g_DeviceContext->PSSetConstantBuffers(5, 1, &g_ShadowBuffer);
}

void BeginSkydome()
{
	g_DeviceContext->OMSetDepthStencilState(g_DepthStateSkydome, 0);
	g_DeviceContext->RSSetState(g_RasterizerStateSkydome);
}

void EndSkydome()
{
	g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable, 0);
	g_DeviceContext->RSSetState(g_RasterizerState);
}



void EndFrame()
{
	g_SwapChain->Present(1, 0);
}


void SetDepthEnable(bool Enable)
{
	if (Enable)
		g_DeviceContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	else
		g_DeviceContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);

}


void SetATCEnable(bool Enable)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		g_DeviceContext->OMSetBlendState(g_BlendStateATC, blendFactor, 0xffffffff);
	else
		g_DeviceContext->OMSetBlendState(g_BlendState, blendFactor, 0xffffffff);

}

void SetAlphaBlendTransparent()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendState, blendFactor, 0xffffffff);
}

void SetAlphaBlendAdd()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_DeviceContext->OMSetBlendState(g_BlendStateAdd, blendFactor, 0xffffffff);
}

void SetWorldViewProjection2D()
{
	SetWorldMatrix(XMMatrixIdentity());
	SetViewMatrix(XMMatrixIdentity());

	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH(
		0.0f,
		(float)GetBackBufferWidth(),
		(float)GetBackBufferHeight(),
		0.0f,
		0.0f,
		1.0f
	);
	SetProjectionMatrix(projection);
}


void SetWorldMatrix(XMMATRIX WorldMatrix)
{
	XMFLOAT4X4 worldf;
	XMStoreFloat4x4(&worldf, XMMatrixTranspose(WorldMatrix));
	g_DeviceContext->UpdateSubresource(g_WorldBuffer, 0, NULL, &worldf, 0, 0);
}

void SetViewMatrix(XMMATRIX ViewMatrix)
{
	XMFLOAT4X4 viewf;
	XMStoreFloat4x4(&viewf, XMMatrixTranspose(ViewMatrix));
	g_DeviceContext->UpdateSubresource(g_ViewBuffer, 0, NULL, &viewf, 0, 0);
}

void SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	XMFLOAT4X4 projectionf;
	XMStoreFloat4x4(&projectionf, XMMatrixTranspose(ProjectionMatrix));
	g_DeviceContext->UpdateSubresource(g_ProjectionBuffer, 0, NULL, &projectionf, 0, 0);

}



void SetMaterial(MATERIAL Material)
{
	g_DeviceContext->UpdateSubresource(g_MaterialBuffer, 0, NULL, &Material, 0, 0);
}

void SetLight(LIGHT Light)
{
	g_DeviceContext->UpdateSubresource(g_LightBuffer, 0, NULL, &Light, 0, 0);
}


void SetTexCoord(TEXCOORD TexCoord)
{
	g_DeviceContext->UpdateSubresource(g_TexCoordBuffer, 0, NULL, &TexCoord, 0, 0);
}

void SetDiffuseColor(const XMFLOAT4& Color)
{
	PS_DIFFUSE diffuse{};
	diffuse.DiffuseColor = Color;
	g_DeviceContext->UpdateSubresource(g_DiffuseBuffer, 0, NULL, &diffuse, 0, 0);
}

void SetAmbientColor(const XMFLOAT4& Color)
{
	PS_AMBIENT ambient{};
	ambient.AmbientColor = Color;
	g_DeviceContext->UpdateSubresource(g_AmbientBuffer, 0, NULL, &ambient, 0, 0);
}

void SetDirectionalLight(const XMFLOAT4& Direction, const XMFLOAT4& Color)
{
	PS_DIRECTIONAL_LIGHT light{};
	light.DirectionalWorldVector = Direction;
	light.DirectionalColor = Color;
	g_DeviceContext->UpdateSubresource(g_DirectionalLightBuffer, 0, NULL, &light, 0, 0);
}

void SetSpecular(const XMFLOAT3& EyePosW, float Power, const XMFLOAT4& Color)
{
	PS_SPECULAR specular{};
	specular.EyePosW = EyePosW;
	specular.SpecularPower = Power;
	specular.SpecularColor = Color;
	g_DeviceContext->UpdateSubresource(g_SpecularBuffer, 0, NULL, &specular, 0, 0);
}

void SetPointLight(const PS_POINT_LIGHT& PointLight)
{
	g_DeviceContext->UpdateSubresource(g_PointLightBuffer, 0, NULL, &PointLight, 0, 0);
}

void SetShadowConstant(const SHADOW_CONSTANT& ShadowConstant)
{
	g_DeviceContext->UpdateSubresource(g_ShadowBuffer, 0, NULL, &ShadowConstant, 0, 0);
}



void CreateVertexShader2D(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
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

void CreateVertexShader3D(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
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



void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
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


