#include "field.h"
#include "sprite.h"

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�
---------------------------------------------------------------------------*/

// �e�N�X�`���p�̃O���[�o���ϐ�
static ID3D11ShaderResourceView* g_Texture = NULL;

/*---------------------------------------------------------------------------
	����������
---------------------------------------------------------------------------*/
void InitField(void)
{
	// �e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\GameBG.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);
	// 	LoadFromWICFile(L"asset\\texture\\BG.png", WIC_FLAGS_NONE, &metadata, image);
	// CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	// assert(g_Texture);
}

/*---------------------------------------------------------------------------
	�I������
---------------------------------------------------------------------------*/
void UninitField(void)
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
void UpdateField(void)
{

}

/*---------------------------------------------------------------------------
	�`�揈��
---------------------------------------------------------------------------*/
void DrawField(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.0f, 
			   XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
}
/*---------------------------------------------------------------------------
	���_�f�[�^�ݒ�
---------------------------------------------------------------------------*/
void SetFieldVertex(void)
{

}