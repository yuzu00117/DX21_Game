#include "piece.h"
#include "sprite.h"
#include "block.h"
#include "effect.h"
#include "sound.h"
#include "score.h"
#include "fade.h"

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

static int g_SENo = 0;

static BLOCK g_Block[BLOCK_ROWS][BLOCK_COLS];

// Pieceの状態
enum BLOCK_STATE
{
    BLOCK_STATE_IDLE,
    BLOCK_STATE_ERASE_IDLE,
    BLOCK_STATE_STACK_IDLE,
};
static BLOCK_STATE g_BlockState;
static int g_BlockStateCount;

static int g_ComboCount = 0; // 連鎖回数のカウンタ

/*---------------------------------------------------------------------------
    初期化処理
---------------------------------------------------------------------------*/
void InitBlock(void)
{
    // テクスチャ読み込み
    TexMetadata metadata;
    ScratchImage image;

    // ファイル名
	LoadFromWICFile(L"asset\\texture\\green.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[0]);
	assert(g_Texture[0]);

	LoadFromWICFile(L"asset\\texture\\yellow.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[1]);
	assert(g_Texture[1]);

	LoadFromWICFile(L"asset\\texture\\red.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture[2]);
	assert(g_Texture[2]);

	LoadFromWICFile(L"asset\\texture\\blue.png", WIC_FLAGS_NONE, &metadata, image);
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
    g_BlockState = BLOCK_STATE_IDLE;
    g_BlockStateCount = 0;

    g_SENo = LoadSound((char*)"asset/sounds/bomb000.wav");
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
    switch (g_BlockState)
    {
    case BLOCK_STATE_IDLE:
        break;
    case BLOCK_STATE_ERASE_IDLE:
        g_BlockStateCount++;
        if (g_BlockStateCount > 30)
        {
            StackBlock();
        }
        break;
    case BLOCK_STATE_STACK_IDLE:
    g_BlockStateCount++;
    if (g_BlockStateCount > 30)
    {
        EraseBlock();
    }
        break;
    
    default:
        break;
    }

    // すでにブロックが最上段にあるかチェック
    for (int x = 0; x < BLOCK_COLS; x++)
    {
        if (g_Block[0][x].Enable)
        {
            // 最上段にブロックがあるので、ゲーム終了処理に移行
            FadeOut();

            if (IsFadeOutFinish())
            {
                SetScene(SCENE_RESULT); // リザルトシーンに切り替え
            }
            return;
        }
    }

    for (int y = 0; y < BLOCK_ROWS; y++) {
        for (int x = 0; x < BLOCK_COLS; x++) {
            if (g_Block[y][x].Flashing) {
                // 点滅カウントを更新
                g_Block[y][x].FlashCount++;

                // 点滅処理 (例: 5フレームごとに表示/非表示を切り替える)
                if (g_Block[y][x].FlashCount % 10 < 5) {
                    g_Block[y][x].Enable = true;  // 表示
                } else {
                    g_Block[y][x].Enable = false; // 非表示
                }

                // 一定回数点滅したら削除
                if (g_Block[y][x].FlashCount > 30) {
                    g_Block[y][x].Erase = false;
                    g_Block[y][x].Enable = false;
                    g_Block[y][x].Flashing = false;

                    // エフェクト生成（オプション）
                    XMFLOAT2 position;
                    position.x = x * PIECE_WIDTH + SCREEN_WIDTH * 0.5f - (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH;
                    position.y = y * PIECE_HEIGHT + SCREEN_HEIGHT * 0.5f - (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT;
                    CreateEffect(position);
                }
            }
        }
    }
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

BLOCK GetBlock(int x, int y)
{
    return g_Block[y][x];
}

void EraseBlock()
{
    bool erase = false;

//---------------------------------------------------------
// 横方向チェック
    int type = -1;
    int count = 0;

    for (int y = 0; y < BLOCK_ROWS; y++)
    {
        for (int x = 0; x < BLOCK_COLS; x++)
        {
            if (g_Block[y][x].Enable)
            {
                if (g_Block[y][x].Type == type)
                {
                    count++;

                    if (count >= 2)
                    {
                        for (int i = x; i > x - 3; i--)
                        {
                            g_Block[y][i].Erase = true;
                        }
                        erase = true;
                    }
                }
                else
                {
                    type = g_Block[y][x].Type;
                    count = 0;
                }
            }
            else
            {
                type = -1;
                count = 0;
            }
        }
        type = -1;
        count = 0;
    }

//---------------------------------------------------------  


//---------------------------------------------------------
// 縦方向チェック
    for (int x = 0; x < BLOCK_COLS; x++)
    {
        type = -1;
        count = 0;

        for (int y = 0; y < BLOCK_ROWS; y++)
        {
            if (g_Block[y][x].Enable)
            {
                if (g_Block[y][x].Type == type)
                {
                    count++;

                    if (count >= 2)
                    {
                        for (int i = y; i > y - 3; i--)
                        {
                            g_Block[i][x].Erase = true;
                        }
                        erase = true;
                    }
                }
                else
                {
                    type = g_Block[y][x].Type;
                    count = 0;
                }
            }
            else
            {
                type = -1;
                count = 0;
            }
        }
    }

//---------------------------------------------------------    

//---------------------------------------------------------    
// 斜め方向チェック
 for (int start = 0; start < BLOCK_ROWS + BLOCK_COLS - 1; start++)
    {
        int x = max(0, start - BLOCK_ROWS + 1);
        int y = max(0, BLOCK_ROWS - 1 - start);
        type = -1;
        count = 0;

        while (x < BLOCK_COLS && y < BLOCK_ROWS)
        {
            if (g_Block[y][x].Enable)
            {
                if (g_Block[y][x].Type == type)
                {
                    count++;

                    if (count >= 2)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            g_Block[y - i][x - i].Erase = true;
                        }

                        erase = true;
                    }
                }
                else
                {
                    type = g_Block[y][x].Type;
                    count = 0;
                }
            }
            else
            {
                type = -1;
                count = 0;
            }

            x++;
            y++;
        }
    }
//?
    for (int start = 0; start < BLOCK_ROWS + BLOCK_COLS - 1; start++)
    {
        int x = min(BLOCK_COLS - 1, start);
        int y = max(0, start - BLOCK_COLS + 1);
        type = -1;
        count = 0;

        while (x >= 0 && y < BLOCK_ROWS)
        {
            if (g_Block[y][x].Enable)
            {
                if (g_Block[y][x].Type == type)
                {
                    count++;

                    if (count >= 2)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            g_Block[y - i][x + i].Erase = true;
                        }

                        erase = true;
                    }
                }
                else
                {
                    type = g_Block[y][x].Type;
                    count = 0;
                }
            }
            else
            {
                type = -1;
                count = 0;
            }

            x--;
            y++;
        }
    }


//--------------------------------------------------------- 
// ブロック削除
    for (int y = 0; y < BLOCK_ROWS; y++)
    {
        for (int x = 0; x < BLOCK_COLS; x++)
        {
            if (g_Block[y][x].Erase)
            {
                // 削除の代わりに点滅を開始
                g_Block[y][x].Flashing = true;
                g_Block[y][x].FlashCount = 0;  // 点滅カウントを初期化
                for (int y = 0; y < BLOCK_ROWS; y++) {
        for (int x = 0; x < BLOCK_COLS; x++) {
            if (g_Block[y][x].Flashing) {
                // 点滅カウントを更新
                g_Block[y][x].FlashCount++;

                // 点滅処理 (例: 5フレームごとに表示/非表示を切り替える)
                if (g_Block[y][x].FlashCount % 10 < 5) {
                    g_Block[y][x].Enable = true;  // 表示
                } else {
                    g_Block[y][x].Enable = false; // 非表示
                }

                // 一定回数点滅したら削除
                if (g_Block[y][x].FlashCount > 30) {
                    g_Block[y][x].Erase = false;
                    g_Block[y][x].Enable = false;
                    g_Block[y][x].Flashing = false;

                    // エフェクト生成（オプション）
                    XMFLOAT2 position;
                    position.x = x * PIECE_WIDTH + SCREEN_WIDTH * 0.5f - (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH;
                    position.y = y * PIECE_HEIGHT + SCREEN_HEIGHT * 0.5f - (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT;
                    CreateEffect(position);

                    g_BlockState = BLOCK_STATE_STACK_IDLE;
                }
            }
        }
    }
            }
        }
    }


    if (erase == true)
    {
        g_BlockState = BLOCK_STATE_ERASE_IDLE;
        g_BlockStateCount = 0;

        // 効果音再生
        PlaySound(g_SENo, 0);
        // 連鎖回数に応じてスコアを加算
        g_ComboCount++;
        AddScore(g_ComboCount);
    }
    else
    {   
        // 連鎖が途切れた場合、カウンタをリセット
        g_ComboCount = 0;
        CreatePiece();
        g_BlockState = BLOCK_STATE_IDLE;
        g_BlockStateCount = 0;
    }
}

void StackBlock()
{
    bool stack = false;


    for (int y = BLOCK_ROWS - 1; y > 0; y--)
    {
        for (int x = 0; x < BLOCK_COLS; x++)
        {
            if (g_Block[y][x].Enable == false)
            {
                for (int ys = y - 1; ys >= 0; ys--)
                {
                    if (g_Block[ys][x].Enable == true)
                    {
                        g_Block[y][x] = g_Block[ys][x];
                        g_Block[ys][x].Enable = false;

                        stack = true;
                        
                        break;
                    }   
                }
            }   
        } 
    }

    if (stack == true)
    {
        g_BlockState = BLOCK_STATE_STACK_IDLE;
        g_BlockStateCount = 0;
    }
    else
    {
        CreatePiece();
        g_BlockState = BLOCK_STATE_IDLE;
        g_BlockStateCount = 0;
    }
}