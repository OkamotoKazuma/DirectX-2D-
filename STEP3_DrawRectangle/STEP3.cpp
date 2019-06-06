#include <windows.h>
#include <d3dx9.h>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pDevice;
LPDIRECT3DTEXTURE9 pTexture;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3d(HWND);
VOID InitRender();
VOID DrawRectangle();
VOID FreeDx();

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT icmdShow)
{
	MSG msg;
	static char szAppName[] = "STEP3";
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
		0, 0, 640, 480, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	if (FAILED(InitD3d(hWnd)))
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
				DrawRectangle();
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
	if (FAILED(D3DXCreateTextureFromFile(pDevice, "10123.png", &pTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}
}

// 透明処理の設定
VOID InitRender()
{
	// テクスチャの設定
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // アルファブレンディング
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); // 透明処理
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // 透過処理
}

// 矩形の設定
// 頂点情報
struct CUSTOM_VERTEX
{
	// 座標
	FLOAT x, y, z, rhw;
	// 頂点の色
	DWORD color;
	// テクスチャ座標
	FLOAT tu, tv;
};

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

CUSTOM_VERTEX Position[] = {
			{170.0f, 110.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255,255,255,0), 0.0f, 0.0f}, //左上
			{470.0f, 110.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255,255,255,0), 1.0f, 0.0f}, //右上
			{470.0f, 410.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255,255,255,0), 1.0f, 1.0f}, //右下
			{170.0f, 410.0f, 0.0f, 1.0f, D3DCOLOR_RGBA(255,255,255,0), 0.0f, 1.0f}  //左下
};

// 矩形を描画する関数
VOID DrawRectangle()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	if (SUCCEEDED(pDevice->BeginScene()))
	{
		pDevice->SetFVF(FVF_CUSTOM);
		InitRender();
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Position, sizeof(CUSTOM_VERTEX));
		pDevice->SetTexture(0, pTexture);
		pDevice->EndScene();
	}
	pDevice->Present(NULL, NULL, NULL, NULL);
}

// 作成したDirectXオブジェクトの開放
VOID FreeDx()
{
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pD3d);
}
