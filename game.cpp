#include "game.h"
#include "sprite.h"
#include "field.h"
#include "piece.h"
#include "effect.h"
#include "score.h"
#include "block.h"
#include "sound.h"

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�
---------------------------------------------------------------------------*/
static int g_BGMNo = 0;

/*---------------------------------------------------------------------------
	����������
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
	�I������
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
	�X�V����
---------------------------------------------------------------------------*/
void UpdateGame(void)
{
	UpdatePiece();
	UpdateBlock();
	UpdateEffect();
	UpdateScore();
}

/*---------------------------------------------------------------------------
	�`�揈��
---------------------------------------------------------------------------*/
void DrawGame(void)
{
    DrawField();
	DrawPiece();
	DrawBlock();
	DrawEffect();
	DrawScore();
}