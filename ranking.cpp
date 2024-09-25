#include "ranking.h"
#include "sprite.h"

/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/

// テクスチャ用のグローバル変数
static ID3D11ShaderResourceView* g_Texture = NULL;

int g_ranking;

/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void Initranking(void)
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\number.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void Uninitranking(void)
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
void Updateranking(void)
{

}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void Drawranking(void)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	int ranking = g_ranking;

	for (int i = 0; i < 3; i++) // スコアを3桁表示
	{
		DrawSpriteAnim(XMFLOAT2(300.0f - 70.0f * i, 400.0f), 0.0f, 
		            	XMFLOAT2(100.0f, 100.0f), 5, 5, ranking % 10);

		ranking /= 10;	
	}
}
/*---------------------------------------------------------------------------
	頂点データ設定
---------------------------------------------------------------------------*/
void Addranking(int ranking)
{
	g_ranking += ranking;
}