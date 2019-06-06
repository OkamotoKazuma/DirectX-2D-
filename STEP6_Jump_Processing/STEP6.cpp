#include <windows.h>
#include <d3dx9.h>
#include <dinput.h>
#include "GameLib_Move.h"
#include "GameLib_Texture.h"

#define CharacterMax 2

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pDevice;
LPDIRECT3DTEXTURE9 pTexture;
LPDIRECTINPUT8 pDinput = NULL; //DirectInput�I�u�W�F�N�g�̃|�C���^
LPDIRECTINPUTDEVICE8 pKeyDevice = NULL; //DirectInput�f�o�C�X�I�u�W�F�N�g�̃|�C���^
LPD3DXFONT m_pFont;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3d(HWND);
HRESULT InitDinput(HWND);
VOID InitRender();
VOID DrawRectangle();
VOID AppProcess();
VOID FreeDx();

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT icmdShow)
{
	MSG msg;
	static char szAppName[] = "STEP5";
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	HWND hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		0, 0, 640, 520, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	if (FAILED(InitD3d(hWnd)))
	{
		return 0;
	}

	// �_�C���N�g�C���v�b�g�̏������֐����Ă�
	if (FAILED(InitDinput(hWnd)))
	{
		return 0;
	}

	//	���C�����[�v
	DWORD SyncPrev = timeGetTime();
	DWORD SyncCurr;
	ZeroMemory(&msg, sizeof(msg));
	timeBeginPeriod(1);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			SyncCurr = timeGetTime();
			if (SyncCurr - SyncPrev >= 1000 / 60)
			{
				AppProcess();
				SyncPrev = SyncCurr;
			}
		}
		Sleep(1);
	}
	timeEndPeriod(1);
	FreeDx();
	return (INT)msg.wParam;
}

// �E�B���h�v���V�[�W���֐�
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch ((CHAR)wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

// �_�C���N�g3D�̏������֐�
HRESULT InitD3d(HWND hWnd)
{
	//�uDirect3D�v�I�u�W�F�N�g�̍쐬
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3D�̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�uDIRECT3D�f�o�C�X�v�I�u�W�F�N�g�̍쐬
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;

	if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_MIXED_VERTEXPROCESSING,
		&d3dpp, &pDevice)))
	{
		MessageBox(0, "HAL���[�h��DIRECT3D�f�o�C�X���쐬�ł��܂���\nREF���[�h�ōĎ��s���܂�", NULL, MB_OK);
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &pDevice)))
		{
			MessageBox(0, "DIRECT3D�f�o�C�X�̍쐬�Ɏ��s���܂���", NULL, MB_OK);
			return E_FAIL;
		}
	}

	//�u�e�N�X�`���I�u�W�F�N�g�̍쐬�v
	//for (int i = 0; i < CharacterMax; i++)
	{
		if (FAILED(D3DXCreateTextureFromFile(pDevice, "bomb.png", &pTexture)))
		{
			MessageBox(0, "�e�N�X�`���̍쐬�Ɏ��s���܂���", "", MB_OK);
			return E_FAIL;
		}
	}
}

// �_�C���N�g�C���v�b�g�̏������֐�
HRESULT InitDinput(HWND hWnd)
{
	HRESULT hr;
	//�uDirectInput�v�I�u�W�F�N�g�̍쐬
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID * *)& pDinput, NULL)))
	{
		return hr;
	}
	//�uDirectInput �f�o�C�X�v�I�u�W�F�N�g�̍쐬
	if (FAILED(hr = pDinput->CreateDevice(GUID_SysKeyboard,
		&pKeyDevice, NULL)))
	{
		return hr;
	}
	// �f�o�C�X���L�[�{�[�h�ɐݒ�
	if (FAILED(hr = pKeyDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return hr;
	}
	// �������x���̐ݒ�
	if (FAILED(hr = pKeyDevice->SetCooperativeLevel(
		hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) //��r���Ńo�b�N�O���E���h�̋������x��
	{
		return hr;
	}
	// �f�o�C�X���u�擾�v����
	pKeyDevice->Acquire(); // �A�N�Z�X���𓾂�
	return S_OK;
}

CUSTOMVERTEX Pos_BlackBomb[4] = {
	// �����e
			{170.0f, 380.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.0f}, //����
			{270.0f, 380.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.0f}, //�E��
			{270.0f, 480.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.1f}, //�E��
			{170.0f, 480.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.1f}  //����
};

CUSTOMVERTEX Pos_GoldBomb[4] = {
	// �����e
			{50.0f, 110.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.3f},
			{150.0f, 110.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.3f},
			{150.0f, 210.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.4f},
			{50.0f, 210.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.4f}
};

AccMotion BlackBomb = { 20.0f, 0.5f, TRUE};

// �A�v���P�[�V���������֐�
VOID AppProcess()
{
	DrawRectangle();

	//INT g_KeyInfo[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT };
	//Input_State g_InputState[KEY_INFO::MAX_KEY_INFO];

	// �L�[�{�[�h�ŉ�����Ă���L�[�𒲂ׁA�Ή���������Ɉړ�������
	HRESULT hr = pKeyDevice->Acquire();
	if ((hr == DI_OK) || (hr == S_FALSE))
	{
		BYTE diks[256]; // �L�[�{�[�h�̉������
		pKeyDevice->GetDeviceState(sizeof(diks), &diks);

		FLOAT speed = 4.0f;

		if (diks[DIK_LEFT] & 0x80) // ���ړ�
		{
			Move_Left(speed, Pos_BlackBomb, Pos_GoldBomb);
		}

		if (diks[DIK_RIGHT] & 0x80) // �E�ړ�
		{
			Move_Right(speed, Pos_BlackBomb, Pos_GoldBomb);
		}

		if (diks[DIK_UP] & 0x80) // ��ւ̃W�����v
		{
			if (BlackBomb.JumpFlag == FALSE)
			{
				Dec_Up(BlackBomb.I_Speed, BlackBomb.Acc, Pos_BlackBomb, Pos_GoldBomb);
				if (BlackBomb.I_Speed < 0.0f)
				{
					BlackBomb.JumpFlag = TRUE;
				}
			}
			else if (BlackBomb.JumpFlag == TRUE)
			{
				Acc_Down(BlackBomb.I_Speed, BlackBomb.Acc, Pos_BlackBomb, Pos_GoldBomb);
				if (Pos_BlackBomb[2].y >= 480)
				{
					BlackBomb.JumpFlag = FALSE;
				}
			}
		}

		if (diks[DIK_DOWN] & 0x80) // ���ړ�
		{
			if (Pos_BlackBomb[2].y < 480)
			{
				Move_Down(speed, Pos_BlackBomb, Pos_GoldBomb);
			}
		}
	}
	pDevice->Present(NULL, NULL, NULL, NULL);
}

// ��`��`�悷��֐�
VOID DrawRectangle()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	if (SUCCEEDED(pDevice->BeginScene()))
	{
		pDevice->SetFVF(FVF_CUSTOM);
		InitRender(); // GameLib_Texture�Q��
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Pos_BlackBomb, sizeof(CUSTOMVERTEX));
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Pos_GoldBomb, sizeof(CUSTOMVERTEX));
		pDevice->EndScene();
	}
}

// �쐬����DirectX�I�u�W�F�N�g�̊J��
VOID FreeDx()
{
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pD3d);
	pKeyDevice->Unacquire(); // �A�N�Z�X��������
	SAFE_RELEASE(pKeyDevice);
	SAFE_RELEASE(pDinput);
}
