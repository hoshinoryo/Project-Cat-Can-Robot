#pragma once


#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)



struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
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


// for 3D constant buffer
struct PS_DIFFUSE
{
	XMFLOAT4 DiffuseColor;
};

struct PS_AMBIENT
{
	XMFLOAT4 AmbientColor;
};

struct PS_DIRECTIONAL_LIGHT
{
	XMFLOAT4 DirectionalWorldVector;
	XMFLOAT4 DirectionalColor;
};

struct PS_SPECULAR
{
	XMFLOAT3 EyePosW;
	float SpecularPower;
	XMFLOAT4 SpecularColor;
};

struct POINT_LIGHT
{
	XMFLOAT3 PosW;
	float Range;
	XMFLOAT4 Color;
};

struct PS_POINT_LIGHT
{
	POINT_LIGHT PointLight[4];
	int PointLightCount;
	XMFLOAT3 Dummy;
};

struct SHADOW_CONSTANT
{
	XMFLOAT4X4 LightViewProj;
	float ShadowBias;
	float ShadowStrength;
	XMFLOAT2 ShadowTexelSize;
};


void InitRenderer();
void UninitRenderer();

void BeginFrame(); // clear
void EndFrame();

void BeginRenderer2D(); // before 2D drawing
void BeginRenderer3D(); // before 3D drawing

void BeginSkydome();
void EndSkydome();

void SetDepthEnable(bool Enable);
void SetATCEnable(bool Enable);
void SetAlphaBlendTransparent();
void SetAlphaBlendAdd();

void SetWorldViewProjection2D();
void SetWorldMatrix(XMMATRIX WorldMatrix);
void SetViewMatrix(XMMATRIX ViewMatrix);
void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
void SetMaterial(MATERIAL Material);
void SetLight(LIGHT Light);
void SetTexCoord(TEXCOORD TexCoord);

// for 3D constant buffer
void SetDiffuseColor(const XMFLOAT4& Color);
void SetAmbientColor(const XMFLOAT4& Color);
void SetDirectionalLight(const XMFLOAT4& Direction, const XMFLOAT4& Color);
void SetSpecular(const XMFLOAT3& EyePosW, float Power, const XMFLOAT4& Color);
void SetPointLight(const PS_POINT_LIGHT& PointLight);
void SetShadowConstant(const SHADOW_CONSTANT& ShadowConstant);


ID3D11Device* GetDevice(void);
ID3D11DeviceContext* GetDeviceContext(void);

unsigned int GetBackBufferWidth();
unsigned int GetBackBufferHeight();


void CreateVertexShader2D(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
void CreateVertexShader3D(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

