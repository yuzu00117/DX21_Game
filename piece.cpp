#include "time.h"
#include "piece.h"
#include "sprite.h"
#include "keyboard.h"
#include "block.h"

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
static int g_AnimFrame = 0; // 現在のフレーム数

static bool g_OldKeystate_D = false; 	// 過去のスペースキー状態:D
static bool g_OldKeystate_A = false; 	// 過去のスペースキー状態:A
static bool g_OldKeystate_W = false; 	// 過去のスペースキー状態:W
static bool g_OldKeystate_S = false;	// 過去のスペースキー状態:S
static bool g_OldKeystate_Down = false; // 過去のスペースキー状態:S

struct PIECE
{
	XMFLOAT2 Position; // 描画位置
	int Type[3];	   // マークの番号
};
static PIECE g_Piece;

// Pieceの状態
enum PIECE_STATE
{
	PIECE_STATE_IDLE,
	PIECE_STATE_MOVE,
	PIECE_STATE_GROUND_IDLE,
	PIECE_STATE_MISS_IDLE,
};
static PIECE_STATE g_PieceState;
static int g_PieceStateCount;

/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitPiece(void)
{
	ID3D11Device *pDevice = GetDevice();

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

	// 疑似乱数のシードを設定する関数
	srand((unsigned int)time(NULL));

	// Pieceの初期化
	CreatePiece();
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void UninitPiece(void)
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
void UpdatePiece(void)
{
	switch (g_PieceState)
	{
	case PIECE_STATE_IDLE:
		break;
	case PIECE_STATE_MOVE:
		MovePiece();
		break;
	case PIECE_STATE_GROUND_IDLE:
		g_PieceStateCount++;
		if (g_PieceStateCount > 60)
		{
			g_PieceState = PIECE_STATE_IDLE;
			g_PieceStateCount = 0;

			EraseBlock();
		}
		break;
	case PIECE_STATE_MISS_IDLE:
		g_PieceStateCount++;
		if (g_PieceStateCount > 60)
		{
			SetScene(SCENE_RESULT);
		}
		break;
	default:
		break;
	}
}

void CreatePiece(void)
{
	// Pieceの初期化
	g_Piece.Position = XMFLOAT2(SCREEN_WIDTH / 2 + PIECE_WIDTH * 0.5f, 100.0f);
	// 乱数でマーク設定
	g_Piece.Type[0] = rand() % 4;
	g_Piece.Type[1] = rand() % 4;
	g_Piece.Type[2] = rand() % 4;

	// Pieceの状態を初期化
	g_PieceState = PIECE_STATE_MOVE;
	g_PieceStateCount = 0;
}

// ピース移動
void MovePiece(void)
{
	BLOCK block; // ブロック構造体
	int x, y;
	static float MaxStageRight = 765.0f;
	static float MaxStageLeft = 515.0f;

	// ピースが地面に設置するまで落下させる
	if (g_Piece.Position.y < SCREEN_HEIGHT - 150.0f)
	{
		g_Piece.Position.y += 1.0f;
	}

	// キートリガー(キーを押した瞬間だけ処理)
	bool Keystate_D = Keyboard_IsKeyDown(KK_D);
	bool Keystate_A = Keyboard_IsKeyDown(KK_A);
	bool Keystate_W = Keyboard_IsKeyDown(KK_W);
	bool Keystate_S = Keyboard_IsKeyDown(KK_S);
	bool Keystate_Down = Keyboard_IsKeyDown(KK_DOWN);

	// 落下速度の上昇
	if (!g_OldKeystate_Down && Keystate_Down)
	{
		g_Piece.Position.y += 2.0f;
	}

	// 右移動
	if (!g_OldKeystate_D && Keystate_D)
	{
		// 右側の衝突判定
		x = (int)((g_Piece.Position.x + PIECE_WIDTH - SCREEN_WIDTH * 0.5f + 
		(BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
		y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f +
		(BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

		block = GetBlock(x,y);

		// xが枠内の場合のみ移動させる
		if (block.Enable == false && g_Piece.Position.x < MaxStageRight)
			g_Piece.Position.x += PIECE_HEIGHT;
	}
	g_OldKeystate_D = Keystate_D;

	// 左移動
	if (!g_OldKeystate_A && Keystate_A)
	{
		// 左側の衝突判定
		x = (int)((g_Piece.Position.x - PIECE_WIDTH - SCREEN_WIDTH * 0.5f + 
		(BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
		y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f +
		(BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

		block = GetBlock(x,y);

		// xが枠内の場合のみ移動させる
		if (block.Enable == false && g_Piece.Position.x > MaxStageLeft)
			g_Piece.Position.x -= PIECE_HEIGHT;
	}
	g_OldKeystate_A = Keystate_A;

	// マークのスクロール
	if (!g_OldKeystate_W && Keystate_W)
	{
		int OldPiece = g_Piece.Type[0];
		g_Piece.Type[0] = g_Piece.Type[1];
		g_Piece.Type[1] = g_Piece.Type[2];
		g_Piece.Type[2] = OldPiece;
	}
	g_OldKeystate_W = Keystate_W;

	if (!g_OldKeystate_S && Keystate_S)
	{
		int OldPiece = g_Piece.Type[2];
		g_Piece.Type[2] = g_Piece.Type[1];
		g_Piece.Type[1] = g_Piece.Type[0];
		g_Piece.Type[0] = OldPiece;
	}
	g_OldKeystate_S = Keystate_S;

	// 衝突判定
	bool ground = false;

		// 地面との衝突判定
		if (g_Piece.Position.y + PIECE_HEIGHT * 2 >
			SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT)
	{
		ground = true;
	}

	// ブロックとの衝突判定
	x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
	y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

	block = GetBlock(x, y);

	if (block.Enable)
	{
		ground = true;
	}
	
	// 接地したら
	if (ground == true)
	{
		for (int i = 0; i < 3; i++)
		{
			x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
			y = (int)((g_Piece.Position.y + PIECE_HEIGHT * i - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);
			SetBlock(x, y, g_Piece.Type[i]);
		}
		// ステート変更
		g_PieceState = PIECE_STATE_GROUND_IDLE;
	}
}

void DrawPiece(void)
{
	if (g_PieceState != PIECE_STATE_MOVE)
		return;

	// 複数のテクスチャ設定やスプライト描画を処理
	for (int i = 0; i < 3; i++)
	{
		// テクスチャ設定
		int Type = g_Piece.Type[i];
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[Type]);

		// スプライト描画
		DrawSprite(XMFLOAT2(g_Piece.Position.x, g_Piece.Position.y + PIECE_HEIGHT * i),
				   0.0f,
				   XMFLOAT2(PIECE_WIDTH, PIECE_HEIGHT));
	}
}