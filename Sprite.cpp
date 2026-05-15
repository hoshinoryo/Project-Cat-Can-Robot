#include "Main.h"
#include "Renderer2D.h"
#include "Renderer_Manager.h"
#include "Sprite.h"


ID3D11Buffer* g_VertexBuffer{};
ID3D11VertexShader* g_VertexShader{};
ID3D11PixelShader* g_PixelShader{};
ID3D11InputLayout* g_VertexLayout{};

// D3D device and device context
static ID3D11Device* g_Device{};
static ID3D11DeviceContext* g_DeviceContext{};

static	XMFLOAT2	ScrollOffset;


void	UpdateScrollOffset(float x, float y)
{
	ScrollOffset.x += x;
	ScrollOffset.y += y;

	if (ScrollOffset.y > 0.0f)
	{
		ScrollOffset.y = 0.0f;
	}
}

void InitSprite()
{
	g_Device = RendererManager_GetDevice();
	g_DeviceContext = RendererManager_GetDeviceContext();

	VERTEX_2D vertex[4];

	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_2D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	g_Device->CreateBuffer(&bd, &sd, &g_VertexBuffer);


	Renderer2D_CreateVS(&g_VertexShader, &g_VertexLayout, "Shader\\UnlitTextureVS.cso");
	Renderer2D_CreatePS(&g_PixelShader, "Shader\\UnlitTexturePS.cso");

	ScrollOffset.x = 0.0f;
	ScrollOffset.y = 0.0f;

}

void UninitSprite()
{
	g_VertexBuffer->Release();
	g_VertexLayout->Release();
	g_VertexShader->Release();
	g_PixelShader->Release();
}


XMFLOAT2	GetScrollOffset()
{
	return ScrollOffset;
}
void DrawSprite(bool scr, ID3D11ShaderResourceView* Texture, XMFLOAT2 Position, XMFLOAT2 Scale, float Rotation, XMFLOAT4 Color)
{
	if (scr)
	{
		Position.x -= ScrollOffset.x;
		Position.y -= ScrollOffset.y;
	}
	DrawSpriteTexCoord(false, Texture, Position, Scale, { 0.0f, 0.0f }, { 1.0f, 1.0f }, Rotation, Color);

}


void DrawSpriteTexCoord(bool scr,ID3D11ShaderResourceView* Texture, XMFLOAT2 Position, XMFLOAT2 Scale, XMFLOAT2 TexPosition, XMFLOAT2 TexScale, float Rotation, XMFLOAT4 Color)
{
	if (scr)
	{
		Position.x -= ScrollOffset.x;
		Position.y -= ScrollOffset.y;
	}

	// 入力レイアウト設定
	g_DeviceContext->IASetInputLayout(g_VertexLayout);

	// シェーダ設定
	g_DeviceContext->VSSetShader(g_VertexShader, NULL, 0);
	g_DeviceContext->PSSetShader(g_PixelShader, NULL, 0);


	// マトリクス設定
	Renderer2D_SetWorldViewProjection();

	// ワールドマトリクス設定
	XMMATRIX world, scale, rot, trans;
	scale = XMMatrixScaling(Scale.x, Scale.y, 1.0f);
	rot = XMMatrixRotationZ(Rotation);
	trans = XMMatrixTranslation(Position.x, Position.y, 0.0f);
	world = scale * rot * trans;
	Renderer2D_SetWorldMatrix(world);


	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	g_DeviceContext->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// テクスチャ設定
	g_DeviceContext->PSSetShaderResources(0, 1, &Texture);


	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = Color;
	material.TextureEnable = true;
	Renderer2D_SetMaterial(material);


	// テクスチャ座標設定
	TEXCOORD texcoord{};
	texcoord.Position = TexPosition;
	texcoord.Scale = TexScale;
	Renderer2D_SetTexCoord(texcoord);


	// プリミティブトポロジ設定
	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画
	g_DeviceContext->Draw(4, 0);


}
