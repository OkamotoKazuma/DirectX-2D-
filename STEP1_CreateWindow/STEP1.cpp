#include <windows.h>
#include <d3dx9.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	MSG msg;
	static char szAppName[] = "STEP1";
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc; //�E�B���h�v���V�[�W���ւ̃|�C���^
	wndclass.cbClsExtra = 0; //�E�B���h�N���X�\���̂ւ̒ǉ��̃o�C�g��
	wndclass.cbWndExtra = 0; //�E�B���h�E�C���X�^���X�ւ̒ǉ��̃o�C�g��
	wndclass.hInstance = hInst; //wndproc���܂ރC���X�^���X�ւ̑�1�����̃n���h��
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //�N���X�A�C�R���̃n���h��
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); //�N���X�J�[�\���ւ̃n���h��
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //�N���X�w�i�ւ̃n���h��
	wndclass.lpszMenuName = NULL; //�N���X���j���[�̃��\�[�X��
	wndclass.lpszClassName = szAppName; //�E�B���h�E�N���X���̎w��
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass); // �\���̂̓o�^
	HWND hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW, // �E�B���h�E���, �\�����镶��, �E�B���h�E�̃X�^�C��
		0, 0, 640, 480, NULL, NULL, hInst, NULL); // �\���ʒu(0,0,640,480), �E�B���h�E�T�C�Y(width,height), �C���X�^���X�n���h��
	ShowWindow(hWnd, SW_SHOW); // �����E�B���h�E
	UpdateWindow(hWnd);

	//���C�����[�v
	ZeroMemory(&msg, sizeof(msg));
	timeBeginPeriod(1); //�^�C�}�[�̍ŏ����x��ݒ�
	while (msg.message != WM_QUIT)
	{
		//if(PeekMessage(MSG�|�C���^, ���b�Z�[�W�̎擾, �ŏ��̃��b�Z�[�W�̒l, �Ō�̃��b�Z�[�W�̒l, �������@)
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg); //�������b�Z�[�W�ɕϊ��{���b�Z�[�W�L���[�Ƀ|�X�g
			DispatchMessage(&msg); //window procedure�Ƀ��b�Z�[�W�𑗂�
		}
		Sleep(1);
	}
	timeEndPeriod(1); //�^�C�}�[�̍ŏ����x��߂�
	return (INT)msg.wParam;
}

// �E�B���h�v���V�[�W���֐�
// ��M���b�Z�[�W�̑Ή�(���b�Z�[�W�Ώۂ̃E�B���h�E,���b�Z�[�W���e,���b�Z�[�W�̏ڍ�,���b�Z�[�W�̏ڍ�)
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