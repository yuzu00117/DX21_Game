#include "time.h"
#include "piece.h"
#include "sprite.h"
#include "keyboard.h"
#include "block.h"

/*---------------------------------------------------------------------------
	�}�N����`
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
	�v���g�^�C�v�錾
---------------------------------------------------------------------------*/
void MovePiece(void);

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�  DirectX�֌W�̕ϐ��͑S�ă|�C���^�ϐ��ň���
---------------------------------------------------------------------------*/
static ID3D11ShaderResourceView *g_Texture[4] = {NULL};
static int g_AnimFrame = 0; // ���݂̃t���[����

static bool g_OldKeystate_D = false; 	// �ߋ��̃X�y�[�X�L�[���:D
static bool g_OldKeystate_A = false; 	// �ߋ��̃X�y�[�X�L�[���:A
static bool g_OldKeystate_W = false; 	// �ߋ��̃X�y�[�X�L�[���:W
static bool g_OldKeystate_S = false;	// �ߋ��̃X�y�[�X�L�[���:S
static bool g_OldKeystate_Down = false; // �ߋ��̃X�y�[�X�L�[���:S

struct PIECE
{
	XMFLOAT2 Position; // �`��ʒu
	int Type[3];	   // �}�[�N�̔ԍ�
};
static PIECE g_Piece;

// Piece�̏��
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
	����������
---------------------------------------------------------------------------*/
void InitPiece(void)
{
	ID3D11Device *pDevice = GetDevice();

	// �e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;

	// �t�@�C����
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

	// �^�������̃V�[�h��ݒ肷��֐�
	srand((unsigned int)time(NULL));

	// Piece�̏�����
	CreatePiece();
}

/*---------------------------------------------------------------------------
	�I������
---------------------------------------------------------------------------*/
void UninitPiece(void)
{
	// �����̃e�N�X�`���̊J��
	for (int num = 0; num < 4; num++)
	{
		// �e�N�X�`���̉��
		if (g_Texture[num])
		{
			g_Texture[num]->Release();
			g_Texture[num] = NULL;
		}
	}
}

/*---------------------------------------------------------------------------
	�X�V����
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
	// Piece�̏�����
	g_Piece.Position = XMFLOAT2(SCREEN_WIDTH / 2 + PIECE_WIDTH * 0.5f, 100.0f);
	// �����Ń}�[�N�ݒ�
	g_Piece.Type[0] = rand() % 4;
	g_Piece.Type[1] = rand() % 4;
	g_Piece.Type[2] = rand() % 4;

	// Piece�̏�Ԃ�������
	g_PieceState = PIECE_STATE_MOVE;
	g_PieceStateCount = 0;
}

// �s�[�X�ړ�
void MovePiece(void)
{
	BLOCK block; // �u���b�N�\����
	int x, y;
	static float MaxStageRight = 765.0f;
	static float MaxStageLeft = 515.0f;

	// �s�[�X���n�ʂɐݒu����܂ŗ���������
	if (g_Piece.Position.y < SCREEN_HEIGHT - 150.0f)
	{
		g_Piece.Position.y += 1.0f;
	}

	// �L�[�g���K�[(�L�[���������u�Ԃ�������)
	bool Keystate_D = Keyboard_IsKeyDown(KK_D);
	bool Keystate_A = Keyboard_IsKeyDown(KK_A);
	bool Keystate_W = Keyboard_IsKeyDown(KK_W);
	bool Keystate_S = Keyboard_IsKeyDown(KK_S);
	bool Keystate_Down = Keyboard_IsKeyDown(KK_DOWN);

	// �������x�̏㏸
	if (!g_OldKeystate_Down && Keystate_Down)
	{
		g_Piece.Position.y += 2.0f;
	}

	// �E�ړ�
	if (!g_OldKeystate_D && Keystate_D)
	{
		// �E���̏Փ˔���
		x = (int)((g_Piece.Position.x + PIECE_WIDTH - SCREEN_WIDTH * 0.5f + 
		(BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
		y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f +
		(BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

		block = GetBlock(x,y);

		// x���g���̏ꍇ�݈̂ړ�������
		if (block.Enable == false && g_Piece.Position.x < MaxStageRight)
			g_Piece.Position.x += PIECE_HEIGHT;
	}
	g_OldKeystate_D = Keystate_D;

	// ���ړ�
	if (!g_OldKeystate_A && Keystate_A)
	{
		// �����̏Փ˔���
		x = (int)((g_Piece.Position.x - PIECE_WIDTH - SCREEN_WIDTH * 0.5f + 
		(BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
		y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f +
		(BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

		block = GetBlock(x,y);

		// x���g���̏ꍇ�݈̂ړ�������
		if (block.Enable == false && g_Piece.Position.x > MaxStageLeft)
			g_Piece.Position.x -= PIECE_HEIGHT;
	}
	g_OldKeystate_A = Keystate_A;

	// �}�[�N�̃X�N���[��
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

	// �Փ˔���
	bool ground = false;

		// �n�ʂƂ̏Փ˔���
		if (g_Piece.Position.y + PIECE_HEIGHT * 2 >
			SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT)
	{
		ground = true;
	}

	// �u���b�N�Ƃ̏Փ˔���
	x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
	y = (int)((g_Piece.Position.y + PIECE_HEIGHT * 3 - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);

	block = GetBlock(x, y);

	if (block.Enable)
	{
		ground = true;
	}
	
	// �ڒn������
	if (ground == true)
	{
		for (int i = 0; i < 3; i++)
		{
			x = (int)((g_Piece.Position.x - SCREEN_WIDTH * 0.5f + (BLOCK_COLS * 0.5f - 0.5f) * PIECE_WIDTH) / PIECE_WIDTH);
			y = (int)((g_Piece.Position.y + PIECE_HEIGHT * i - SCREEN_HEIGHT * 0.5f + (BLOCK_ROWS * 0.5f - 0.5f) * PIECE_HEIGHT) / PIECE_HEIGHT);
			SetBlock(x, y, g_Piece.Type[i]);
		}
		// �X�e�[�g�ύX
		g_PieceState = PIECE_STATE_GROUND_IDLE;
	}
}

void DrawPiece(void)
{
	if (g_PieceState != PIECE_STATE_MOVE)
		return;

	// �����̃e�N�X�`���ݒ��X�v���C�g�`�������
	for (int i = 0; i < 3; i++)
	{
		// �e�N�X�`���ݒ�
		int Type = g_Piece.Type[i];
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[Type]);

		// �X�v���C�g�`��
		DrawSprite(XMFLOAT2(g_Piece.Position.x, g_Piece.Position.y + PIECE_HEIGHT * i),
				   0.0f,
				   XMFLOAT2(PIECE_WIDTH, PIECE_HEIGHT));
	}
}