/*----------------------------------------------------------------------
Window�̕\��[main.cpp]
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
 ���C�u�����̃����N
-----------------------------------------------------------------------*/

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

/*----------------------------------------------------------------------
 �萔��`
-----------------------------------------------------------------------*/

#define CLASS_NAME "GameWindow"
#define WINDOW_CAPTION "�E�B���h�E�\������"

/*----------------------------------------------------------------------
 �v���g�^�C�v�錾
-----------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

/*----------------------------------------------------------------------
 �O���[�o���ϐ�
-----------------------------------------------------------------------*/
#ifdef _DEBUG
int g_CountFPS;							// FPS�J�E���^
char g_DebugStr[2048] = WINDOW_CAPTION; // �f�o�b�O�����\���p
#endif

SCENE g_Scene;

/*----------------------------------------------------------------------
 ���C��
-----------------------------------------------------------------------*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	// �E�B���h�E�N���X�̓o�^
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	RegisterClass(&wc);

	RECT rc = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// �E�B���h�E�̍쐬
	// CrateWindow�c�E�B���h�E���쐬����֐�
	HWND hWnd = CreateWindow(
		CLASS_NAME, // �E�B���h�E�̈�ԏ�ɕ\�������Ƃ���
		WINDOW_CAPTION,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, // �E�B���h�E�̈ʒu�ݒ�(x��)
		CW_USEDEFAULT, // �E�B���h�E�̈ʒu�ݒ�(y��)
		rc.right - rc.left, // �E�B���h�E�̕�
		rc.bottom - rc.top, // �E�B���h�E�̍���
		NULL,
		NULL, // hMenu�c���j���[�ւ̃n���h��
		hInstance,
		NULL);

	// DirectX�̏�����(�E�C���h�E���쐬���Ă���s��)
	if (FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	// ���v�v���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	// �t���[���J�E���g������
	timeBeginPeriod(1);								// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime(); // �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�B���h�E�\��(Init()�̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	// ���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ // PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
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
			// �V�X�e���������擾
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000) // 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;

				wsprintf(g_DebugStr, WINDOW_CAPTION);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], "FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);
#endif
				// FPS�𑪒肵��������ۑ�
				dwFPSLastTime = dwCurrentTime;
				// �J�E���g���N���A
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= ((float)1000 / 60)) // 1/60���ƂɎ��s
			{
				// ��������������ۑ�
				dwExecLastTime = dwCurrentTime;

				// �X�V����
				Update();
				// �`�揈��
				Draw();

				// �����񐔂̃J�E���g�����Z
				dwFrameCount++;
			}
		}
	}

	// ����\��߂�
	timeEndPeriod(1);

	// �I������
	Uninit();

	// �I��
	return (int)msg.wParam;
}

// �v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:			 // �L�[����
		if (wParam == VK_ESCAPE) // Escape���������Ƃ�
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}

	case WM_ACTIVATEAPP:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_CLOSE: // �E�B���h�E�����
		if (MessageBox(hWnd, "�{���ɏI�����Ă�낵���ł����H", "�m�F", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
		{
			DestroyWindow(hWnd); // �E�B���h�E�����
		}
		return 0;

	case WM_DESTROY: // �A�v���P�[�V�����I��
		PostQuitMessage(0);
		return 0;
	};

	// ���̑��̃��b�Z�[�W��windows�ɔC����
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*----------------------------------------------------------------------
����������
----------------------------------------------------------------------*/
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_�����O�����̏�����
	InitRenderer(hInstance, hWnd, bWindow);
	InitSprite();
	InitSound(hWnd);
	InitFade();

	g_Scene = SCENE_NONE;
	SetScene(SCENE_TITLE);

	return S_OK;
}

/*----------------------------------------------------------------------
�I������
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
�X�V����
----------------------------------------------------------------------*/
void Update(void)
{
	UpdateFade();

	switch (g_Scene)
	{
	case SCENE_TITLE:
		UpdateTitle(); // �^�C�g������
		break;
	case SCENE_GAME:
		UpdateGame(); // �Q�[������
		break;
	case SCENE_RESULT:
		UpdateResult(); // ���U���g����
		break;
	
	default:
		break;
	}
}

/*----------------------------------------------------------------------
�`�揈��
----------------------------------------------------------------------*/
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);

	// �����ɕ`�揈��
	switch (g_Scene)
	{
	case SCENE_TITLE:
		DrawTitle(); // �^�C�g������
		break;
	case SCENE_GAME:
		DrawGame(); // �Q�[������
		break;
	case SCENE_RESULT:
		DrawResult(); // ���U���g����
		break;
	default:
		break;
	}

	DrawFade();

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}

void SetScene(SCENE Scene)
{
	switch (g_Scene)
	{
	case SCENE_TITLE:
		UninitTitle(); // �^�C�g������
		break;
	case SCENE_GAME:
		UninitGame(); // �Q�[������
		break;
	case SCENE_RESULT:
		UninitResult(); // ���U���g����
		break;
	
	default:
		break;
	}

	g_Scene = Scene;

	switch (g_Scene)
	{
	case SCENE_TITLE:
		InitTitle(); // �^�C�g������
		break;
	case SCENE_GAME:
		InitGame(); // �Q�[������
		break;
	case SCENE_RESULT:
		InitResult(); // ���U���g����
		break;
	
	default:
		break;
	}


}