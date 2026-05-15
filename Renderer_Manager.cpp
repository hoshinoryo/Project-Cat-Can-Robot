
#include <assert.h>

#include "Renderer_Manager.h"
#include "Main.h"

#pragma comment(lib, "d3d11.lib")

static D3D_FEATURE_LEVEL g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

static ID3D11Device* g_Device{};
static ID3D11DeviceContext* g_DeviceContext{};
static IDXGISwapChain* g_SwapChain{};
static ID3D11RenderTargetView* g_RenderTargetView{};
static ID3D11DepthStencilView* g_DepthStencilView{};

static D3D11_TEXTURE2D_DESC g_BackBufferDesc{};
static D3D11_VIEWPORT g_Viewport{};

template<class T>
static void SafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

void RendererManager_Initialize()
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

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
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
		&g_DeviceContext
	);
	assert(SUCCEEDED(hr));

	// レンダーターゲットビュー作成
	ID3D11Texture2D* renderTarget{};
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTarget);
	assert(SUCCEEDED(hr));

	renderTarget->GetDesc(&g_BackBufferDesc);
	hr = g_Device->CreateRenderTargetView(renderTarget, NULL, &g_RenderTargetView);
	assert(SUCCEEDED(hr));

	SafeRelease(renderTarget);

	// デプスステンシルバッファ作成
	ID3D11Texture2D* depthStencil{};
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = g_BackBufferDesc.Width;
	textureDesc.Height = g_BackBufferDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = g_Device->CreateTexture2D(&textureDesc, NULL, &depthStencil);
	assert(SUCCEEDED(hr));

	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = g_Device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &g_DepthStencilView);
	assert(SUCCEEDED(hr));

	SafeRelease(depthStencil);

	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	// ビューポート設定
	g_Viewport.Width = static_cast<FLOAT>(g_BackBufferDesc.Width);
	g_Viewport.Height = static_cast<FLOAT>(g_BackBufferDesc.Height);
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;
	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_DeviceContext->RSSetViewports(1, &g_Viewport);
}

void RendererManager_Finalize()
{
	if (g_DeviceContext)
	{
		g_DeviceContext->ClearState();
	}

	SafeRelease(g_DepthStencilView);
	SafeRelease(g_RenderTargetView);
	SafeRelease(g_SwapChain);
	SafeRelease(g_DeviceContext);
	SafeRelease(g_Device);
}

void RendererManager_BeginFrame()
{
	float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // blue

	g_DeviceContext->ClearRenderTargetView(g_RenderTargetView, clearColor);
	g_DeviceContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_DeviceContext->RSSetViewports(1, &g_Viewport);
}

void RendererManager_EndFrame()
{
	g_SwapChain->Present(1, 0);
}

ID3D11Device* RendererManager_GetDevice(void)
{
	return g_Device;
}

ID3D11DeviceContext* RendererManager_GetDeviceContext(void)
{
	return g_DeviceContext;
}

ID3D11RenderTargetView* RendererManager_GetRenderTargetView()
{
	return g_RenderTargetView;
}

ID3D11DepthStencilView* RendererManager_GetDepthStencilView()
{
	return g_DepthStencilView;
}

unsigned int RendererManager_GetBackBufferWidth()
{
	return g_BackBufferDesc.Width;
}

unsigned int RendererManager_GetBackBufferHeight()
{
	return g_BackBufferDesc.Height;
}
