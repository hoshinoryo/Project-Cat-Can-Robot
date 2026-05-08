#pragma once





struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
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



void InitRenderer();
void UninitRenderer();
void BeginRenderer();
void EndRenderer();

void SetDepthEnable(bool Enable);
void SetATCEnable(bool Enable);
void SetWorldViewProjection2D();
void SetWorldMatrix(XMMATRIX WorldMatrix);
void SetViewMatrix(XMMATRIX ViewMatrix);
void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
void SetMaterial(MATERIAL Material);
void SetLight(LIGHT Light);
void SetTexCoord(TEXCOORD TexCoord);

ID3D11Device* GetDevice(void);
ID3D11DeviceContext* GetDeviceContext(void);



void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

