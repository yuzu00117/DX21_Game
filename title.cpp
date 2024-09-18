#include "title.h"
#include "sprite.h"
#include "keyboard.h"
#include "fade.h"

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�
---------------------------------------------------------------------------*/

// �e�N�X�`���p�̃O���[�o���ϐ�
static ID3D11ShaderResourceView* g_Texture = NULL;

static bool g_OldKeystate_ENTER = false; // �ߋ��̃X�y�[�X�L�[���:ENTER

/*---------------------------------------------------------------------------
	����������
---------------------------------------------------------------------------*/
void InitTitle(void)
{
	// �e�N�X�`���ǂݍ���
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
	�I������
---------------------------------------------------------------------------*/
void UninitTitle(void)
{
	// �e�N�X�`���̊J��
	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
    }
}

/*---------------------------------------------------------------------------
	�X�V����
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
	�`�揈��
---------------------------------------------------------------------------*/
void DrawTitle(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.0f, 
			   XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
}
/*---------------------------------------------------------------------------
	���_�f�[�^�ݒ�
---------------------------------------------------------------------------*/
void SetTitleVertex(void)
{

}