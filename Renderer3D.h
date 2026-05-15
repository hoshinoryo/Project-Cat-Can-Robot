#pragma once

#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)

#include <d3d11.h>
#include <DirectXMath.h>

struct VERTEX_3D
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT2 TexCoord;
};

// for 3D constant buffer
struct PS_DIFFUSE
{
	DirectX::XMFLOAT4 DiffuseColor;
};

struct PS_AMBIENT
{
	DirectX::XMFLOAT4 AmbientColor;
};

struct PS_DIRECTIONAL_LIGHT
{
	DirectX::XMFLOAT4 DirectionalWorldVector;
	DirectX::XMFLOAT4 DirectionalColor;
};

struct PS_SPECULAR
{
	DirectX::XMFLOAT3 EyePosW;
	float SpecularPower;
	DirectX::XMFLOAT4 SpecularColor;
};

struct POINT_LIGHT
{
	DirectX::XMFLOAT3 PosW;
	float Range;
	DirectX::XMFLOAT4 Color;
};

struct PS_POINT_LIGHT
{
	POINT_LIGHT PointLight[4];
	int PointLightCount;
	DirectX::XMFLOAT3 Dummy;
};

struct SHADOW_CONSTANT
{
	DirectX::XMFLOAT4X4 LightViewProj;
	float ShadowBias;
	float ShadowStrength;
	DirectX::XMFLOAT2 ShadowTexelSize;
};

void Renderer3D_Initialize();
void Renderer3D_Finalize();

void Renderer3D_BeginSkydome();
void Renderer3D_EndSkydome();

void Renderer3D_Begin(); // before 3D drawing

// matrix transformation
void Renderer3D_SetWorldMatrix(DirectX::XMMATRIX WorldMatrix);
void Renderer3D_SetViewMatrix(DirectX::XMMATRIX ViewMatrix);
void Renderer3D_SetProjectionMatrix(DirectX::XMMATRIX ProjectionMatrix);

// for 3D constant buffer
void Renderer3D_SetDiffuseColor(const DirectX::XMFLOAT4& Color);
void Renderer3D_SetAmbientColor(const DirectX::XMFLOAT4& Color);
void Renderer3D_SetDirectionalLight(const DirectX::XMFLOAT4& Direction, const DirectX::XMFLOAT4& Color);
void Renderer3D_SetSpecular(const DirectX::XMFLOAT3& EyePosW, float Power, const DirectX::XMFLOAT4& Color);
void Renderer3D_SetPointLight(const PS_POINT_LIGHT& PointLight);
void Renderer3D_SetShadowConstant(const SHADOW_CONSTANT& ShadowConstant);

// directX status
void Renderer3D_SetDepthEnable(bool Enable);
void Renderer3D_SetAlphaBlendTransparent();
void Renderer3D_SetAlphaBlendAdd();

void Renderer3D_CreateVS(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
void Renderer3D_CreatePS(ID3D11PixelShader** PixelShader, const char* FileName);