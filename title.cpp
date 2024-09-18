#include "title.h"
#include "sprite.h"
#include "keyboard.h"
#include "fade.h"

/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/

// テクスチャ用のグローバル変数
static ID3D11ShaderResourceView* g_Texture = NULL;

static bool g_OldKeystate_ENTER = false; // 過去のスペースキー状態:ENTER

/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitTitle(void)
{
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	// LoadFromWICFile(L"asset\\texture\\gametitle.png", WIC_FLAGS_NONE, &metadata, image);
	// CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	// assert(g_Texture);
		LoadFromWICFile(L"asset\\texture\\title.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void UninitTitle(void)
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
void UpdateTitle(void)
{
	bool Keystate_ENTER = Keyboard_IsKeyDown(KK_ENTER);

	if (!g_OldKeystate_ENTER && Keystate_ENTER)
	{
		FadeOut();
	}

	if (IsFadeOutFinish())
	{
		SetScene(SCENE_GAME);
	}
	
	g_OldKeystate_ENTER = Keystate_ENTER;
}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void DrawTitle(void)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.0f, 
			   XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
}
/*---------------------------------------------------------------------------
	頂点データ設定
---------------------------------------------------------------------------*/
void SetTitleVertex(void)
{

}