#pragma once

#include <DirectXMath.h>



void InitSprite();
void UninitSprite();
void DrawSprite(
	bool scr,
	ID3D11ShaderResourceView*
	Texture,
	DirectX::XMFLOAT2 Position,
	DirectX::XMFLOAT2 Scale,
	float Rotation = 0.0f,
	DirectX::XMFLOAT4 Color = {1.0f, 1.0f, 1.0f, 1.0f}
);
void DrawSpriteTexCoord(
	bool scr,
	ID3D11ShaderResourceView* Texture,
	DirectX::XMFLOAT2 Position,
	DirectX::XMFLOAT2 Scale,
	DirectX::XMFLOAT2 TexPosition,
	DirectX::XMFLOAT2 TexScale,
	float Rotation = 0.0f,
	DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f }
);
void UpdateScrollOffset(float x, float y);
DirectX::XMFLOAT2 GetScrollOffset();


