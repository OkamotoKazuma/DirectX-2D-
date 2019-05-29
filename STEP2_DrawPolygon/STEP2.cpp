#include <windows.h>
#include <d3dx9.h>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } } // �������̊J��

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pDevice;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3d(HWND);
VOID DrawPolygon();
VOID FreeDx();

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	MSG msg;
	static char szAppName[] = "STEP2";
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

	// ���C�����[�v
	DWORD SyncPrev = timeGetTime(); //�ŏ��̎���
	DWORD SyncCurr;
	ZeroMemory(&msg, sizeof(msg));
	timeBeginPeriod(1); //�^�C�}�[�̍ŏ����x��ݒ�
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			SyncCurr = timeGetTime(); //�Ō�̎���
			if (SyncCurr - SyncPrev >= 1000 / 60) // �t���[���ҋ@
			{
				DrawPolygon();
				SyncPrev = SyncCurr;
			}
		}
		Sleep(1);
	}
	timeEndPeriod(1); //�^�C�}�[�̍ŏ����x��߂�
	FreeDx(); // �I�u�W�F�N�g�̊J��
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

	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; //�t�H�[�}�b�g�Ȃ�
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE; //�A�v���P�[�V�����̕\���`��

	if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, // HAL:�n�[�h�E�F�A
		D3DCREATE_MIXED_VERTEXPROCESSING,
		&d3dpp, &pDevice)))
	{
		MessageBox(0, "HAL���[�h��DIRECT3D�f�o�C�X���쐬�ł��܂���\nREF���[�h�ōĎ��s���܂�", NULL, MB_OK);
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, // REF:�\�t�g�E�F�A
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &pDevice)))
		{
			MessageBox(0, "DIRECT3D�f�o�C�X�̍쐬�Ɏ��s���܂���", NULL, MB_OK);
			return E_FAIL;
		}
	}
}

// �|���S���̐ݒ�
// ���_���
struct CUSTOM_VERTEX
{
	// ���W
	float x, y, z, rhw;
	// ���_�̐F
	DWORD color;
};

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

CUSTOM_VERTEX Position[] = {
			{170.0f, 110.0f, 0.0f, 1.0f, 0xffffffff}, // ����
			{470.0f, 110.0f, 0.0f, 1.0f, 0xffffffff}, // �E��
			{170.0f, 410.0f, 0.0f, 1.0f, 0xffffffff}  // ����
};

// �|���S����`�悷��֐�
VOID DrawPolygon()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	if (SUCCEEDED(pDevice->BeginScene())) //�����������ǂ���
	{
		pDevice->SetFVF(FVF_CUSTOM);
		// pDevice->DrawPrimitiveUP(�|���S���̕`����@,�|���S���̐�,���_�f�[�^�̃|�C���^,���_�f�[�^�̃T�C�Y)
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, Position, sizeof(CUSTOM_VERTEX));
		pDevice->EndScene();
	}
	pDevice->Present(NULL, NULL, NULL, NULL); //��ʂ̍X�V
}

// �쐬����DirectX�I�u�W�F�N�g�̊J��
VOID FreeDx()
{
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pD3d);
}
