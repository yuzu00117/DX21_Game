#include "game.h"
#include "sprite.h"
#include "field.h"
#include "piece.h"
#include "effect.h"
#include "score.h"
#include "block.h"
#include "sound.h"

/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/
static int g_BGMNo = 0;

/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitGame(void)
{
    InitField();
	InitPiece();
	InitBlock();
	InitEffect();
	InitScore();

    g_BGMNo = LoadSound((char*)"asset/sounds/sample000.wav");	
	SetVolume(g_BGMNo, 0.1f);
	PlaySound(g_BGMNo, -1);
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void UninitGame(void)
{
	StopSound(g_BGMNo);
	
    UninitField();
	UninitPiece();
	UninitBlock();
	UninitEffect();
	UninitScore();
}

/*---------------------------------------------------------------------------
	更新処理
---------------------------------------------------------------------------*/
void UpdateGame(void)
{
	UpdatePiece();
	UpdateBlock();
	UpdateEffect();
	UpdateScore();
}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void DrawGame(void)
{
    DrawField();
	DrawPiece();
	DrawBlock();
	DrawEffect();
	DrawScore();
}