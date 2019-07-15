#include <windows.h>
#include <d3dx9.h>
#include <dinput.h>
#include "GameLib_Move.h"
#include "GameLib_Texture.h"

#define CharacterMax 2
#define Win_w 640.0f
#define Win_h 480.0f

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pDevice;
LPDIRECT3DTEXTURE9 pTexture;
LPDIRECTINPUT8 pDinput = NULL; //DirectInputオブジェクトのポインタ
LPDIRECTINPUTDEVICE8 pKeyDevice = NULL; //DirectInputデバイスオブジェクトのポインタ
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
	static char szAppName[] = "STEP6";
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
		0, 0, Win_w, Win_h + 40, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	if (FAILED(InitD3d(hWnd)))
	{
		return 0;
	}

	// ダイレクトインプットの初期化関数を呼ぶ
	if (FAILED(InitDinput(hWnd)))
	{
		return 0;
	}

	//	メインループ
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

// ウィンドプロシージャ関数
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

// ダイレクト3Dの初期化関数
HRESULT InitD3d(HWND hWnd)
{
	//「Direct3D」オブジェクトの作成
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3Dの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//「DIRECT3Dデバイス」オブジェクトの作成
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
		MessageBox(0, "HALモードでDIRECT3Dデバイスを作成できません\nREFモードで再試行します", NULL, MB_OK);
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &pDevice)))
		{
			MessageBox(0, "DIRECT3Dデバイスの作成に失敗しました", NULL, MB_OK);
			return E_FAIL;
		}
	}

	//「テクスチャオブジェクトの作成」
	//for (int i = 0; i < CharacterMax; i++)
	{
		if (FAILED(D3DXCreateTextureFromFile(pDevice, "bomb.png", &pTexture)))
		{
			MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
			return E_FAIL;
		}
	}
}

// ダイレクトインプットの初期化関数
HRESULT InitDinput(HWND hWnd)
{
	HRESULT hr;
	//「DirectInput」オブジェクトの作成
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID * *)& pDinput, NULL)))
	{
		return hr;
	}
	//「DirectInput デバイス」オブジェクトの作成
	if (FAILED(hr = pDinput->CreateDevice(GUID_SysKeyboard,
		&pKeyDevice, NULL)))
	{
		return hr;
	}
	// デバイスをキーボードに設定
	if (FAILED(hr = pKeyDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return hr;
	}
	// 協調レベルの設定
	if (FAILED(hr = pKeyDevice->SetCooperativeLevel(
		hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) //非排他でバックグラウンドの協調レベル
	{
		return hr;
	}
	// デバイスを「取得」する
	pKeyDevice->Acquire(); // アクセス権を得る
	return S_OK;
}

CUSTOMVERTEX Pos_BlackBomb[4] = {
	// 黒爆弾
			{170.0f, 380.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.0f}, //左上
			{270.0f, 380.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.0f}, //右上
			{270.0f, 480.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.1f}, //右下
			{170.0f, 480.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.1f}  //左下
};

CUSTOMVERTEX Pos_GoldBomb[4] = {
	// 金爆弾
			{50.0f, 210.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.3f},
			{150.0f, 210.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.3f},
			{150.0f, 310.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.16f, 0.4f},
			{50.0f, 310.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 0), 0.0f, 0.4f}
};

AccMotion BlackBomb = { 20.0f, 0.5f, FALSE };

// アプリケーション処理関数
VOID AppProcess()
{
	DrawRectangle();

	FLOAT speed = 4.0f;

	//INT g_KeyInfo[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT };
	//Input_State g_InputState[KEY_INFO::MAX_KEY_INFO];

	// キーボードで押されているキーを調べ、対応する方向に移動させる
	HRESULT hr = pKeyDevice->Acquire();
	if ((hr == DI_OK) || (hr == S_FALSE))
	{
		BYTE diks[256]; // キーボードの押下情報
		pKeyDevice->GetDeviceState(sizeof(diks), &diks);

		if (diks[DIK_LEFT] & 0x80) // 左移動
		{
			Move_Left(speed, Pos_BlackBomb);
			SinkInto_Left(Pos_BlackBomb, Pos_GoldBomb);
		}

		if (diks[DIK_RIGHT] & 0x80) // 右移動
		{
			Move_Right(speed, Pos_BlackBomb);
			SinkInto_Right(Pos_BlackBomb, Pos_GoldBomb);
		}

		if (diks[DIK_UP] & 0x80) // 上へのジャンプ
		{
			BlackBomb.JumpFlag = TRUE;
		}

		
		if (BlackBomb.JumpFlag == TRUE)
		{
			if ((Pos_BlackBomb[2].y == Pos_GoldBomb[0].y) && (Pos_BlackBomb[0].x < Pos_GoldBomb[1].x && Pos_BlackBomb[1].x > Pos_GoldBomb[0].x))
			{
				BlackBomb.I_Speed = 0.0f;
			}
			else
			{
				Move_Up(BlackBomb.I_Speed, Pos_BlackBomb);
				BlackBomb.I_Speed -= BlackBomb.Acc;

				SinkInto_Up(Pos_BlackBomb, Pos_GoldBomb);
				if (Pos_BlackBomb[0].y == Pos_GoldBomb[2].y)
				{
					BlackBomb.I_Speed = -0.01f;
				}

				SinkInto_Down(Pos_BlackBomb, Pos_GoldBomb);
				WindowCollision_Down(Pos_BlackBomb, &BlackBomb, 100, Win_h);
			}
		}

		pDevice->Present(NULL, NULL, NULL, NULL);
	}
}

// 矩形を描画する関数
VOID DrawRectangle()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	if (SUCCEEDED(pDevice->BeginScene()))
	{
		pDevice->SetFVF(FVF_CUSTOM);
		InitRender(); // GameLib_Texture参照
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Pos_BlackBomb, sizeof(CUSTOMVERTEX));
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Pos_GoldBomb, sizeof(CUSTOMVERTEX));
		pDevice->EndScene();
	}
}

// 作成したDirectXオブジェクトの開放
VOID FreeDx()
{
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pD3d);
	pKeyDevice->Unacquire(); // アクセス権を失う
	SAFE_RELEASE(pKeyDevice);
	SAFE_RELEASE(pDinput);
}
