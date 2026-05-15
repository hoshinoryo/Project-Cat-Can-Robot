#pragma once

#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)

#include <d3d11.h>

void RendererManager_Initialize();
void RendererManager_Finalize();

void RendererManager_BeginFrame(); // clear
void RendererManager_EndFrame();

ID3D11Device* RendererManager_GetDevice(void);
ID3D11DeviceContext* RendererManager_GetDeviceContext(void);

ID3D11RenderTargetView* RendererManager_GetRenderTargetView();
ID3D11DepthStencilView* RendererManager_GetDepthStencilView();

unsigned int RendererManager_GetBackBufferWidth();
unsigned int RendererManager_GetBackBufferHeight();