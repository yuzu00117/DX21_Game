#include "score.h"
#include "sprite.h"

/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/

// テクスチャ用のグローバル変数
static ID3D11ShaderResourceView* g_Texture = NULL;

int g_Score;

/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitScore(void)
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
void UninitScore(void)
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
void UpdateScore(void)
{

}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void DrawScore(void)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	int score = g_Score;

	for (int i = 0; i < 3; i++) // スコアを3桁表示
	{
		DrawSpriteAnim(XMFLOAT2(200.0f - 70.0f * i, 200.0f), 0.0f, 
		            	XMFLOAT2(100.0f, 100.0f), 5, 5, score % 10);

		score /= 10;	
	}
}
/*---------------------------------------------------------------------------
	頂点データ設定
---------------------------------------------------------------------------*/
void AddScore(int Score)
{
	g_Score += Score;
}