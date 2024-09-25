#include "ranking.h"
#include "sprite.h"

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�
---------------------------------------------------------------------------*/

// �e�N�X�`���p�̃O���[�o���ϐ�
static ID3D11ShaderResourceView* g_Texture = NULL;

int g_ranking;

/*---------------------------------------------------------------------------
	����������
---------------------------------------------------------------------------*/
void Initranking(void)
{
	// �e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\number.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);
}

/*---------------------------------------------------------------------------
	�I������
---------------------------------------------------------------------------*/
void Uninitranking(void)
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
void Updateranking(void)
{

}

/*---------------------------------------------------------------------------
	�`�揈��
---------------------------------------------------------------------------*/
void Drawranking(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	int ranking = g_ranking;

	for (int i = 0; i < 3; i++) // �X�R�A��3���\��
	{
		DrawSpriteAnim(XMFLOAT2(300.0f - 70.0f * i, 400.0f), 0.0f, 
		            	XMFLOAT2(100.0f, 100.0f), 5, 5, ranking % 10);

		ranking /= 10;	
	}
}
/*---------------------------------------------------------------------------
	���_�f�[�^�ݒ�
---------------------------------------------------------------------------*/
void Addranking(int ranking)
{
	g_ranking += ranking;
}