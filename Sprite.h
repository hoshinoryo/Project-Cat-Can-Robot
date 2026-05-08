#pragma once




void InitSprite();
void UninitSprite();
void DrawSprite(bool scr, ID3D11ShaderResourceView* Texture, XMFLOAT2 Position, XMFLOAT2 Scale, float Rotation = 0.0f, XMFLOAT4 Color = {1.0f, 1.0f, 1.0f, 1.0f});
void DrawSpriteTexCoord(bool scr, ID3D11ShaderResourceView* Texture, XMFLOAT2 Position, XMFLOAT2 Scale, XMFLOAT2 TexPosition, XMFLOAT2 TexScale, float Rotation = 0.0f, XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f });
void	UpdateScrollOffset(float x, float y);
XMFLOAT2	GetScrollOffset();


