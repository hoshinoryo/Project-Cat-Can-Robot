
#include	"Bg.h"
#include	"Sprite.h"
#include	"Renderer.h"


BGOBJECT	Bg;

void	InitBg()
{
	TexMetadata		metadata;
	ScratchImage	image;
	LoadFromWICFile(L"Asset\\Texture\\Bg.jpg", WIC_FLAGS_NONE, &metadata, image);
	//読み込んだ画像データをDirectXへ渡してテクスチャとして管理させる
	CreateShaderResourceView(GetDevice(), image.GetImages(),
		image.GetImageCount(), metadata, &Bg.TexID);
	//なんか失敗した場合に警告を出す
	assert(Bg.TexID);

}
void	UninitBg()
{

	Bg.TexID->Release();

}
void	UpdateBg()
{

}
void	DrawBg()
{
	XMFLOAT2	position = XMFLOAT2(SCREEN_WIDTH/2,0);
	XMFLOAT4	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	XMFLOAT2	texcoord;
	XMFLOAT2	scroll = GetScrollOffset();//現在のスクロール値
	texcoord.y = (1.0f / (BGSIZE_Y*2)) * (scroll.y * 0.3f);
	texcoord.x = 0.0f;
	XMFLOAT2	texsize = XMFLOAT2(1.0f, 1.0f);

	DrawSpriteTexCoord(false, Bg.TexID, position, { BGSIZE_X, BGSIZE_Y}, texcoord, texsize);



}

