#include "piece.h"
#include "sprite.h"

/*---------------------------------------------------------------------------
    マクロ定義
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
    プロトタイプ宣言
---------------------------------------------------------------------------*/
void MovePiece(void);

/*---------------------------------------------------------------------------
    グローバル変数  DirectX関係の変数は全てポインタ変数で扱う
---------------------------------------------------------------------------*/
static ID3D11ShaderResourceView *g_Texture[4] = {NULL};

struct BLOCK
{
    bool Enable;
    bool Erase;
    int Type;
};
static BLOCK g_Block[BLOCK_ROWS][BLOCK_COLS];

// Pieceの状態
enum BLOCK_STATE
{

};
static BLOCK_STATE g_PieceState;

/*---------------------------------------------------------------------------
    初期化処理
---------------------------------------------------------------------------*/
void InitBlock(void)
{
    // テクスチャ読み込み
    TexMetadata metadata;
    ScratchImage image;

    // ファイル名
    LoadFromWICFile(L"asset\\texture\\Clover.png", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[0]);
    assert(g_Texture[0]);

    LoadFromWICFile(L"asset\\texture\\Diamond.png", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[1]);
    assert(g_Texture[1]);

    LoadFromWICFile(L"asset\\texture\\Heart.png", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[2]);
    assert(g_Texture[2]);

    LoadFromWICFile(L"asset\\texture\\Spade.png", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[3]);
    assert(g_Texture[3]);

    // ブロック配列の初期化
    for (int y = 0; y < BLOCK_ROWS; y++)
    {
        for (int x = 0; x < BLOCK_COLS; x++)
        {
            g_Block[y][x].Enable = false;
            g_Block[y][x].Erase = false;
            g_Block[y][x].Type = rand() % 4;
        }
    }
}

/*---------------------------------------------------------------------------
    終了処理
---------------------------------------------------------------------------*/
void UninitBlock(void)
{
    // 複数のテクスチャの開放
    for (int num = 0; num < 4; num++)
    {
        // テクスチャの解放
        if (g_Texture[num])
        {
            g_Texture[num]->Release();
            g_Texture[num] = NULL;
        }
    }
}

/*---------------------------------------------------------------------------
    更新処理
---------------------------------------------------------------------------*/
void UpdateBlock(void)
{
}

/*---------------------------------------------------------------------------
    描画処理
---------------------------------------------------------------------------*/
void DrawBlock(void)
{
    // ブロックのスプライト描画
    for (int y = 0; y < BLOCK_ROWS; y++)
    {
        for (int x = 0; x < BLOCK_COLS; x++)
        {
            if (g_Block[y][x].Enable == true)
            {
                // テクスチャ設定
                int type = g_Block[y][x].Type;
                GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[type]);

                XMFLOAT2 position;
                position.x = x * PIECE_WIDTH + SCREEN_WIDTH * 0.5f - (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH;
                position.y = y * PIECE_HEIGHT + SCREEN_HEIGHT * 0.5f - (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT;

                DrawSprite(position, 0.0f, XMFLOAT2(PIECE_WIDTH, PIECE_HEIGHT));
            }
        }
    }
}

void SetBlock(int x, int y, int Type)
{
    g_Block[y][x].Enable = true;
    g_Block[y][x].Erase = false;
    g_Block[y][x].Type = Type;
}