#include "polygon.h"
#include "keyboard.h"

/*---------------------------------------------------------------------------
	�}�N����`
---------------------------------------------------------------------------*/
#define NUM_VERTEX 4 //�K�v�Ȓ��_�̐�

/*---------------------------------------------------------------------------
	�v���g�^�C�v�錾
---------------------------------------------------------------------------*/
void SetVertex(void);

/*---------------------------------------------------------------------------
	�O���[�o���ϐ�
---------------------------------------------------------------------------*/
static ID3D11Buffer  *g_VertexBuffer = NULL; //���p�̒��_���
static ID3D11ShaderResourceView* g_Texture = NULL;

// �A�j���[�V�����e�N�X�`���p
static int g_AnimPattern = 0 ; // ���݂̃R�}��
static int g_AnimFrame = 0;    // ���݂̃t���[����

// �L�����N�^�[�ړ��p
XMFLOAT2 g_Position; // �L�����N�^�[�ʒu
float g_Rot; 		 // �L�����N�^�[��]
float g_Scale = 2.0f;		 // �L�����N�^�[�T�C�Y

static bool g_OldKeyStateSpace = false;


struct VERTEX_T
{
	XMFLOAT2 pos;  		// �ʒu�x�N�g��
	XMFLOAT2 vel;  		// ���x�x�N�g��
	XMFLOAT4 color;		// ���_�J���[
	XMFLOAT2 texCoord;	// �e�N�X�`�����W
};

static VERTEX_T g_Vertex[NUM_VERTEX];

// �e�N�X�`�����W�p�ϐ�
float x1, x2;
/*---------------------------------------------------------------------------
	����������
---------------------------------------------------------------------------*/
void InitPolygon(void)
{
	ID3D11Device* pDevice = GetDevice();

	//���_�o�b�t�@����(GPU�̃�����)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	//������
	g_Vertex[0].pos.x = -100.0f;
	g_Vertex[0].pos.y = -100.0f;
	g_Vertex[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[0].texCoord = XMFLOAT2(0.0f, 0.0f);

	g_Vertex[1].pos.x = 100.0f;
	g_Vertex[1].pos.y = -100.0f;
	g_Vertex[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[1].texCoord = XMFLOAT2(1.0f, 0.0f);

	g_Vertex[2].pos.x = -100.0f;
	g_Vertex[2].pos.y = 100.0f;
	g_Vertex[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[2].texCoord = XMFLOAT2(0.0f, 1.0f);

	g_Vertex[3].pos.x = 100.0f;
	g_Vertex[3].pos.y = 100.0f;
	g_Vertex[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[3].texCoord = XMFLOAT2(1.0f, 1.0f);
	
	//���_�o�b�t�@�X�V
	SetVertex();

	// �e�N�X�`���ǂݍ���
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\runningman100.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);

	// �L�����N�^�[���W������
	g_Position.x = 200.0f;
	g_Position.y = 200.0f;
}

/*---------------------------------------------------------------------------
	�I������
---------------------------------------------------------------------------*/
void UninitPolygon(void)
{
	//���_�o�b�t�@�̉��
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

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
void UpdatePolygon(void)
{
	// 10�t���[�����ƂɃA�j���[�V�������X�V
	g_AnimFrame++;
	if (g_AnimFrame >= 10)
	{
		g_AnimFrame = 0;

		g_AnimPattern++;
		g_AnimPattern = g_AnimPattern % 10;
	}

	// �L�����N�^�[�̈ړ��i�㉺���E�j
	if (Keyboard_IsKeyDown(KK_A))
	{
		g_Position.x -= 5.0f;
	}
	else if (Keyboard_IsKeyDown(KK_D))
	{
		g_Position.x += 5.0f;
	}
	else if (Keyboard_IsKeyDown(KK_W))
	{
		g_Position.y -= 5.0f;
	}
	else if (Keyboard_IsKeyDown(KK_S))
	{
		g_Position.y += 5.0f;
	}

	// �L�����N�^�[��]
	if (Keyboard_IsKeyDown(KK_RIGHT))
	{
		g_Rot += 0.1f;
	}
	else if (Keyboard_IsKeyDown(KK_LEFT))
	{
		g_Rot -= 0.1f;
	}

	// �L�����N�^�[�X�P�[��
	if (Keyboard_IsKeyDown(KK_UP))
	{
		g_Scale += 0.1f;
	}
	else if (Keyboard_IsKeyDown(KK_DOWN))
	{
		g_Scale -= 0.1f;
	}

	// �L�[�g���K�[�i�������u�Ԃ̂݁j
	bool keystateSpace = Keyboard_IsKeyDown(KK_SPACE);
	if (keystateSpace == true && g_OldKeyStateSpace == false)
	{
		g_Position.x += 200.0f;
	}
	g_OldKeyStateSpace = keystateSpace;
}

/*---------------------------------------------------------------------------
	�`�揈��
---------------------------------------------------------------------------*/
void DrawPolygon(void)
{
	//���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//�}�g���N�X�ݒ�
	//SetWorldViewProjection2D(); //���W��2D�ϊ�

	// �v���W�F�N�V�����}�g���N�X�ݒ�
	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH
				 (0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	SetProjectionMatrix(projection);

	// �r���[�}�g���N�X�ݒu
	XMMATRIX view;
	view = XMMatrixIdentity();
	SetViewMatrix(view);

	// �ړ��E��]�}�g���N�X�ݒ�
	XMMATRIX world, rot, scale, trans;
	scale = XMMatrixScaling(g_Scale, g_Scale, 1.0f);
	rot = XMMatrixRotationZ(g_Rot); // ���W�A���p
	trans = XMMatrixTranslation(g_Position.x, g_Position.y, 0.0f);
	world = scale * rot * trans;
	SetWorldMatrix(world);

	//�v���~�e�B�u�g�{���W�ݒ�                                          LINESTRIP�ELINELIST
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�}�e���A���ݒ�(���N��Ɍ����)
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	int x, y;
	x = g_AnimPattern % 5;
	y = g_AnimPattern / 5;
	g_Vertex[0].texCoord = XMFLOAT2(1.0f / 5.0f * x,		1.0f / 2.0f * y);
	g_Vertex[1].texCoord = XMFLOAT2(1.0f / 5.0f * (x + 1), 	1.0f / 2.0f * y);
	g_Vertex[2].texCoord = XMFLOAT2(1.0f / 5.0f * x, 		1.0f / 2.0f * (y + 1));
	g_Vertex[3].texCoord = XMFLOAT2(1.0f / 5.0f * (x + 1), 	1.0f / 2.0f * (y + 1));
	
	
	SetVertex();
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	//�|���S���`��
	GetDeviceContext()->Draw(NUM_VERTEX, 0);
}

/*---------------------------------------------------------------------------
	���_�f�[�^�ݒ�
---------------------------------------------------------------------------*/
void SetVertex(void)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	//���_�o�b�t�@�̃f�[�^���X�V����
	for (int i = 0; i < NUM_VERTEX; i++)
	{
		vertex[i].Position = XMFLOAT3(g_Vertex[i].pos.x, g_Vertex[i].pos.y, 0.0f);
		vertex[i].Diffuse = g_Vertex[i].color;
		vertex[i].TexCoord = g_Vertex[i].texCoord;
	}

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}