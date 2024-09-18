#include "fade.h"
#include "sprite.h"
#include "keyboard.h"

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�
---------------------------------------------------------------------------*/

// �e�N�X�`���p�̃O���[�o���ϐ�
static ID3D11ShaderResourceView* g_Texture = NULL;

enum FADE
{
    FADE_NONE,
    FADE_OUT,
    FADE_IN
};

static FADE g_Fade = FADE_IN;
static float g_FadeAlpha = 1.0f;
static float g_FadeSpeed = 0.06f;
static bool g_FadeOutFinish = false;

/*---------------------------------------------------------------------------
	����������
---------------------------------------------------------------------------*/
void InitFade(void)
{
	// �e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	// LoadFromWICFile(L"asset\\texture\\GameBG.png", WIC_FLAGS_NONE, &metadata, image);
	// CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	// assert(g_Texture);
		LoadFromWICFile(L"asset\\texture\\Fade.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);
}

/*---------------------------------------------------------------------------
	�I������
---------------------------------------------------------------------------*/
void UninitFade(void)
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
void UpdateFade(void)
{
    if (g_Fade == FADE_OUT)
    {
        g_FadeAlpha += g_FadeSpeed;

        if (g_FadeAlpha > 1.0f)
        {
            g_FadeAlpha = 1.0f;
            g_Fade = FADE_IN;
            g_FadeOutFinish = true;
        }
    }
    else if (g_Fade == FADE_IN)
    {
        g_FadeOutFinish = false;

        g_FadeAlpha -= g_FadeSpeed;

        if (g_FadeAlpha < 0.0f)
        {
            g_FadeAlpha = 0.0f;
            g_Fade = FADE_NONE;
        }
    }

}

/*---------------------------------------------------------------------------
	�`�揈��
---------------------------------------------------------------------------*/
void DrawFade(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.0f, 
			   XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), g_FadeAlpha);
}


/*---------------------------------------------------------------------------
	�t�F�[�h����
---------------------------------------------------------------------------*/
void FadeOut(void)
{
    if (g_Fade == FADE_NONE)
    {
        g_Fade = FADE_OUT;
        g_FadeOutFinish = false;
    }
}

bool IsFadeOutFinish(void)
{
    return g_FadeOutFinish;
}