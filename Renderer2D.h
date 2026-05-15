#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


struct VERTEX_2D
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT2 TexCoord;
};


struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	BOOL		TextureEnable;
	float		Dummy[2];
};



struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];
	XMFLOAT4	Direction;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
};


struct TEXCOORD
{
	XMFLOAT2	Position;
	XMFLOAT2	Scale;
};


void Renderer2D_Initialize();
void Renderer2D_Finalize();

void Renderer2D_Begin(); // before 2D drawing

void Renderer2D_SetWorldViewProjection();
void Renderer2D_SetWorldMatrix(DirectX::XMMATRIX WorldMatrix);
void Renderer2D_SetViewMatrix(DirectX::XMMATRIX ViewMatrix);
void Renderer2D_SetProjectionMatrix(DirectX::XMMATRIX ProjectionMatrix);

void Renderer2D_SetMaterial(const MATERIAL& Material);
void Renderer2D_SetLight(const LIGHT& Light);
void Renderer2D_SetTexCoord(const TEXCOORD& TexCoord);

void Renderer2D_SetDepthEnable(bool Enable);
void Renderer2D_SetAlphaBlendTransparent();
void Renderer2D_SetAlphaBlendAdd();
void Renderer2D_SetATCEnable(bool Enable);

void Renderer2D_CreateVS(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
void Renderer2D_CreatePS(ID3D11PixelShader** PixelShader, const char* FileName);

