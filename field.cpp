#include "field.h"
#include "sprite.h"

/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/

// テクスチャ用のグローバル変数
static ID3D11ShaderResourceView* g_Texture = NULL;

/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitField(void)
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\GameBG.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);
	// 	LoadFromWICFile(L"asset\\texture\\BG.png", WIC_FLAGS_NONE, &metadata, image);
	// CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	// assert(g_Texture);
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void UninitField(void)
{
	// テクスチャの開放
	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
    }
}

/*---------------------------------------------------------------------------
	更新処理
---------------------------------------------------------------------------*/
void UpdateField(void)
{

}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void DrawField(void)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.0f, 
			   XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
}
/*---------------------------------------------------------------------------
	頂点データ設定
---------------------------------------------------------------------------*/
void SetFieldVertex(void)
{

}