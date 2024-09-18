/*----------------------------------------------------------------------
Windowの表示[main.cpp]
								Author :
								Date   :
-----------------------------------------------------------------------*/
#include <Windows.h>
#include "renderer.h"
#include "sprite.h"
#include "polygon.h"
#include "title.h"
#include "keyboard.h"
#include "sound.h"
#include "game.h"
#include "result.h"
#include "fade.h"

/*----------------------------------------------------------------------
 ライブラリのリンク
-----------------------------------------------------------------------*/

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

/*----------------------------------------------------------------------
 定数定義
-----------------------------------------------------------------------*/

#define CLASS_NAME "GameWindow"
#define WINDOW_CAPTION "ウィンドウ表示処理"

/*----------------------------------------------------------------------
 プロトタイプ宣言
-----------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

/*----------------------------------------------------------------------
 グローバル変数
-----------------------------------------------------------------------*/
#ifdef _DEBUG
int g_CountFPS;							// FPSカウンタ
char g_DebugStr[2048] = WINDOW_CAPTION; // デバッグ文字表示用
#endif

SCENE g_Scene;

/*----------------------------------------------------------------------
 メイン
-----------------------------------------------------------------------*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	// ウィンドウクラスの登録
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	RegisterClass(&wc);

	RECT rc = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウの作成
	// CrateWindow…ウィンドウを作成する関数
	HWND hWnd = CreateWindow(
		CLASS_NAME, // ウィンドウの一番上に表示されるところ
		WINDOW_CAPTION,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, // ウィンドウの位置設定(x軸)
		CW_USEDEFAULT, // ウィンドウの位置設定(y軸)
		rc.right - rc.left, // ウィンドウの幅
		rc.bottom - rc.top, // ウィンドウの高さ
		NULL,
		NULL, // hMenu…メニューへのハンドル
		hInstance,
		NULL);

	// DirectXの初期化(ウインドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	// 時計計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	// フレームカウント初期化
	timeBeginPeriod(1);								// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime(); // システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウィンドウ表示(Init()の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ // PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// システム時刻を取得
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000) // 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;

				wsprintf(g_DebugStr, WINDOW_CAPTION);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], "FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);
#endif
				// FPSを測定した時刻を保存
				dwFPSLastTime = dwCurrentTime;
				// カウントをクリア
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= ((float)1000 / 60)) // 1/60ごとに実行
			{
				// 処理した時刻を保存
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				Update();
				// 描画処理
				Draw();

				// 処理回数のカウントを加算
				dwFrameCount++;
			}
		}
	}

	// 分解能を戻す
	timeEndPeriod(1);

	// 終了処理
	Uninit();

	// 終了
	return (int)msg.wParam;
}

// プロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:			 // キー押下
		if (wParam == VK_ESCAPE) // Escapeを押したとき
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}

	case WM_ACTIVATEAPP:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_CLOSE: // ウィンドウを閉じる
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
		{
			DestroyWindow(hWnd); // ウィンドウを閉じる
		}
		return 0;

	case WM_DESTROY: // アプリケーション終了
		PostQuitMessage(0);
		return 0;
	};

	// その他のメッセージはwindowsに任せる
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*----------------------------------------------------------------------
初期化処理
----------------------------------------------------------------------*/
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// レンダリング処理の初期化
	InitRenderer(hInstance, hWnd, bWindow);
	InitSprite();
	InitSound(hWnd);
	InitFade();

	g_Scene = SCENE_NONE;
	SetScene(SCENE_TITLE);

	return S_OK;
}

/*----------------------------------------------------------------------
終了処理
----------------------------------------------------------------------*/
void Uninit(void)
{
	SetScene(SCENE_NONE);

	UninitFade();
	UninitSprite();
	UninitSound();
	UninitRenderer();
}

/*----------------------------------------------------------------------
更新処理
----------------------------------------------------------------------*/
void Update(void)
{
	UpdateFade();

	switch (g_Scene)
	{
	case SCENE_TITLE:
		UpdateTitle(); // タイトル処理
		break;
	case SCENE_GAME:
		UpdateGame(); // ゲーム処理
		break;
	case SCENE_RESULT:
		UpdateResult(); // リザルト処理
		break;
	
	default:
		break;
	}
}

/*----------------------------------------------------------------------
描画処理
----------------------------------------------------------------------*/
void Draw(void)
{
	// バックバッファクリア
	Clear();

	// 2D描画なので深度無効
	SetDepthEnable(false);

	// ここに描画処理
	switch (g_Scene)
	{
	case SCENE_TITLE:
		DrawTitle(); // タイトル処理
		break;
	case SCENE_GAME:
		DrawGame(); // ゲーム処理
		break;
	case SCENE_RESULT:
		DrawResult(); // リザルト処理
		break;
	default:
		break;
	}

	DrawFade();

	// バックバッファ、フロントバッファ入れ替え
	Present();
}

void SetScene(SCENE Scene)
{
	switch (g_Scene)
	{
	case SCENE_TITLE:
		UninitTitle(); // タイトル処理
		break;
	case SCENE_GAME:
		UninitGame(); // ゲーム処理
		break;
	case SCENE_RESULT:
		UninitResult(); // リザルト処理
		break;
	
	default:
		break;
	}

	g_Scene = Scene;

	switch (g_Scene)
	{
	case SCENE_TITLE:
		InitTitle(); // タイトル処理
		break;
	case SCENE_GAME:
		InitGame(); // ゲーム処理
		break;
	case SCENE_RESULT:
		InitResult(); // リザルト処理
		break;
	
	default:
		break;
	}


}