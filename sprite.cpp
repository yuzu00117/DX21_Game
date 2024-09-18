#include "sprite.h"
#include "keyboard.h"

/*---------------------------------------------------------------------------
	マクロ定義
---------------------------------------------------------------------------*/
#define NUM_VERTEX 4 //必要な頂点の数

/*---------------------------------------------------------------------------
	プロトタイプ宣言
---------------------------------------------------------------------------*/
void SetVertexSprite(void);
/*---------------------------------------------------------------------------
	グローバル変数
---------------------------------------------------------------------------*/
static ID3D11Buffer  *g_VertexBuffer = NULL; //線用の頂点情報

struct VERTEX_T
{
	XMFLOAT2 pos;  		// 位置ベクトル
	XMFLOAT2 vel;  		// 速度ベクトル
	XMFLOAT4 color;		// 頂点カラー
	XMFLOAT2 texCoord;	// テクスチャ座標
};

static VERTEX_T g_Vertex[NUM_VERTEX];

/*
// テクスチャ座標用変数
float x1, x2;
*/
/*---------------------------------------------------------------------------
	初期化処理
---------------------------------------------------------------------------*/
void InitSprite(void)
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
	g_Vertex[0].pos.x = -0.5f;
	g_Vertex[0].pos.y = -0.5f;
	g_Vertex[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[0].texCoord = XMFLOAT2(0.0f, 0.0f);

	g_Vertex[1].pos.x = 0.5f;
	g_Vertex[1].pos.y = -0.5f;
	g_Vertex[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[1].texCoord = XMFLOAT2(1.0f, 0.0f);

	g_Vertex[2].pos.x = -0.5f;
	g_Vertex[2].pos.y = 0.5f;
	g_Vertex[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[2].texCoord = XMFLOAT2(0.0f, 1.0f);

	g_Vertex[3].pos.x = 0.5f;
	g_Vertex[3].pos.y = 0.5f;
	g_Vertex[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[3].texCoord = XMFLOAT2(1.0f, 1.0f);
	
	//頂点バッファ更新
	SetVertexSprite();
}

/*---------------------------------------------------------------------------
	終了処理
---------------------------------------------------------------------------*/
void UninitSprite(void)
{
	//頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

/*---------------------------------------------------------------------------
	描画処理
---------------------------------------------------------------------------*/
void DrawSprite(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, float Alpha)
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
	scale = XMMatrixScaling(Scale.x, Scale.y, 1.0f);
	rot = XMMatrixRotationZ(Rotation); // ラジアン角
	trans = XMMatrixTranslation(Position.x, Position.y, 0.0f);
	world = scale * rot * trans;
	SetWorldMatrix(world);

	//プリミティブトボロジ設定                                          LINESTRIP・LINELIST
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//マテリアル設定(半年後に現れる)
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, Alpha);
	SetMaterial(material);

	g_Vertex[0].texCoord = XMFLOAT2(0.0f, 0.0f);
	g_Vertex[1].texCoord = XMFLOAT2(1.0f, 0.0f);
	g_Vertex[2].texCoord = XMFLOAT2(0.0f, 1.0f);
	g_Vertex[3].texCoord = XMFLOAT2(1.0f, 1.0f);

	SetVertexSprite();
	//ポリゴン描画
	GetDeviceContext()->Draw(NUM_VERTEX, 0);
}

void DrawSpriteAnim(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, int AnimCols, int AnimRows, int AnimPattern)
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
	scale = XMMatrixScaling(Scale.x, Scale.y, 1.0f);
	rot = XMMatrixRotationZ(Rotation); // ラジアン角
	trans = XMMatrixTranslation(Position.x, Position.y, 0.0f);
	world = scale * rot * trans;
	SetWorldMatrix(world);

	//プリミティブトボロジ設定                                          LINESTRIP・LINELIST
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//マテリアル設定(半年後に現れる)
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// アニメーション
	int x, y;
	x = AnimPattern % AnimCols;
	y = AnimPattern / AnimCols;
	g_Vertex[0].texCoord = XMFLOAT2(1.0f / AnimCols * x,		1.0f / AnimRows * y);
	g_Vertex[1].texCoord = XMFLOAT2(1.0f / AnimCols * (x + 1), 	1.0f / AnimRows * y);
	g_Vertex[2].texCoord = XMFLOAT2(1.0f / AnimCols * x, 		1.0f / AnimRows * (y + 1));
	g_Vertex[3].texCoord = XMFLOAT2(1.0f / AnimCols * (x + 1), 	1.0f / AnimRows * (y + 1));
	
	
	SetVertexSprite();

	//ポリゴン描画
	GetDeviceContext()->Draw(NUM_VERTEX, 0);
}

/*---------------------------------------------------------------------------
	頂点データ設定
---------------------------------------------------------------------------*/
void SetVertexSprite(void)
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