#include "polygon.h"
#include "keyboard.h"

/*---------------------------------------------------------------------------
	マクロ定義
---------------------------------------------------------------------------*/
#define NUM_VERTEX 4 //必要な頂点の数

/*---------------------------------------------------------------------------
	プロトタイプ宣言
---------------------------------------------------------------------------*/
void SetVertex(void);

/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/
static ID3D11Buffer  *g_VertexBuffer = NULL; //線用の頂点情報
static ID3D11ShaderResourceView* g_Texture = NULL;

// アニメーションテクスチャ用
static int g_AnimPattern = 0 ; // 現在のコマ数
static int g_AnimFrame = 0;    // 現在のフレーム数

// キャラクター移動用
XMFLOAT2 g_Position; // キャラクター位置
float g_Rot; 		 // キャラクター回転
float g_Scale = 2.0f;		 // キャラクターサイズ

static bool g_OldKeyStateSpace = false;


struct VERTEX_T
{
	XMFLOAT2 pos;  		// 位置ベクトル
	XMFLOAT2 vel;  		// 速度ベクトル
	XMFLOAT4 color;		// 頂点カラー
	XMFLOAT2 texCoord;	// テクスチャ座標
};

static VERTEX_T g_Vertex[NUM_VERTEX];

// テクスチャ座標用変数
float x1, x2;
/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitPolygon(void)
{
	ID3D11Device* pDevice = GetDevice();

	//頂点バッファ生成(GPUのメモリ)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	//初期化
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
	
	//頂点バッファ更新
	SetVertex();

	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\runningman100.png", WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &g_Texture);
	assert(g_Texture);

	// キャラクター座標初期化
	g_Position.x = 200.0f;
	g_Position.y = 200.0f;
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void UninitPolygon(void)
{
	//頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの開放
	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}
}

/*---------------------------------------------------------------------------
	更新処理
---------------------------------------------------------------------------*/
void UpdatePolygon(void)
{
	// 10フレームごとにアニメーションを更新
	g_AnimFrame++;
	if (g_AnimFrame >= 10)
	{
		g_AnimFrame = 0;

		g_AnimPattern++;
		g_AnimPattern = g_AnimPattern % 10;
	}

	// キャラクターの移動（上下左右）
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

	// キャラクター回転
	if (Keyboard_IsKeyDown(KK_RIGHT))
	{
		g_Rot += 0.1f;
	}
	else if (Keyboard_IsKeyDown(KK_LEFT))
	{
		g_Rot -= 0.1f;
	}

	// キャラクタースケール
	if (Keyboard_IsKeyDown(KK_UP))
	{
		g_Scale += 0.1f;
	}
	else if (Keyboard_IsKeyDown(KK_DOWN))
	{
		g_Scale -= 0.1f;
	}

	// キートリガー（押した瞬間のみ）
	bool keystateSpace = Keyboard_IsKeyDown(KK_SPACE);
	if (keystateSpace == true && g_OldKeyStateSpace == false)
	{
		g_Position.x += 200.0f;
	}
	g_OldKeyStateSpace = keystateSpace;
}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void DrawPolygon(void)
{
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//マトリクス設定
	//SetWorldViewProjection2D(); //座標の2D変換

	// プロジェクションマトリクス設定
	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH
				 (0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	SetProjectionMatrix(projection);

	// ビューマトリクス設置
	XMMATRIX view;
	view = XMMatrixIdentity();
	SetViewMatrix(view);

	// 移動・回転マトリクス設定
	XMMATRIX world, rot, scale, trans;
	scale = XMMatrixScaling(g_Scale, g_Scale, 1.0f);
	rot = XMMatrixRotationZ(g_Rot); // ラジアン角
	trans = XMMatrixTranslation(g_Position.x, g_Position.y, 0.0f);
	world = scale * rot * trans;
	SetWorldMatrix(world);

	//プリミティブトボロジ設定                                          LINESTRIP・LINELIST
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//マテリアル設定(半年後に現れる)
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
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	//ポリゴン描画
	GetDeviceContext()->Draw(NUM_VERTEX, 0);
}

/*---------------------------------------------------------------------------
	頂点データ設定
---------------------------------------------------------------------------*/
void SetVertex(void)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	//頂点バッファのデータを更新する
	for (int i = 0; i < NUM_VERTEX; i++)
	{
		vertex[i].Position = XMFLOAT3(g_Vertex[i].pos.x, g_Vertex[i].pos.y, 0.0f);
		vertex[i].Diffuse = g_Vertex[i].color;
		vertex[i].TexCoord = g_Vertex[i].texCoord;
	}

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}