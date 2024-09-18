#include "sprite.h"
#include "effect.h"

struct EFFECT
{
    bool Enable;
    XMFLOAT2 Position;
    int FrameCount;
};

#define EFFECT_MAX 100
static EFFECT g_Effect[EFFECT_MAX];

static ID3D11ShaderResourceView* g_Texture{};

void InitEffect()
{
    // �e�N�X�`���ǂݍ���
    TexMetadata metadata;
    ScratchImage image;
    
    

    // �t�@�C����
    LoadFromWICFile(L"asset\\effects\\Explosion.png", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
    assert(g_Texture);

    // �G�t�F�N�g������
    for (int i = 0; i < EFFECT_MAX; i++)
    {
        g_Effect[i].Enable = false;
    }
    
}

void UninitEffect()
{
	g_Texture->Release();
}

void UpdateEffect()
{
    for (int i = 0; i < EFFECT_MAX; i++)
    {
        if (g_Effect[i].Enable)
        {
            g_Effect[i].FrameCount++;

            if (g_Effect[i].FrameCount > 30)
            {
                g_Effect[i].Enable = false;
            }
            
        }
        
    }
    
}

void DrawEffect()
{
    // �e�N�X�`���ݒ�
    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

    for (int i = 0; i < EFFECT_MAX; i++)
    {
        if (g_Effect[i].Enable)
        {
            DrawSpriteAnim(g_Effect[i].Position, 0.0f, XMFLOAT2(PIECE_WIDTH, PIECE_HEIGHT),
            4, 4, g_Effect[i].FrameCount / 2);
        }
    }
}

void CreateEffect(XMFLOAT2 Position)
{
    for (int i = 0; i < EFFECT_MAX; i++)
    {
        if (g_Effect[i].Enable == false)
        {
            g_Effect[i].Enable = true;
            g_Effect[i].Position = Position;
            g_Effect[i].FrameCount = 0;

            break;
        }
        
    }
    
}