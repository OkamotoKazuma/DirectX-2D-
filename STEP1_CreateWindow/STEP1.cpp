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
	wndclass.lpfnWndProc = WndProc; //ウィンドプロシージャへのポインタ
	wndclass.cbClsExtra = 0; //ウィンドクラス構造体への追加のバイト数
	wndclass.cbWndExtra = 0; //ウィンドウインスタンスへの追加のバイト数
	wndclass.hInstance = hInst; //wndprocを含むインスタンスへの第1引数のハンドル
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //クラスアイコンのハンドル
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); //クラスカーソルへのハンドル
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //クラス背景へのハンドル
	wndclass.lpszMenuName = NULL; //クラスメニューのリソース名
	wndclass.lpszClassName = szAppName; //ウィンドウクラス名の指定
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass); // 構造体の登録
	HWND hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW, // ウィンドウ情報, 表示する文字, ウィンドウのスタイル
		0, 0, 640, 480, NULL, NULL, hInst, NULL); // 表示位置(0,0,640,480), ウィンドウサイズ(width,height), インスタンスハンドル
	ShowWindow(hWnd, SW_SHOW); // 初期ウィンドウ
	UpdateWindow(hWnd);

	//メインループ
	ZeroMemory(&msg, sizeof(msg));
	timeBeginPeriod(1); //タイマーの最小精度を設定
	while (msg.message != WM_QUIT)
	{
		//if(PeekMessage(MSGポインタ, メッセージの取得, 最初のメッセージの値, 最後のメッセージの値, 処理方法)
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg); //文字メッセージに変換＋メッセージキューにポスト
			DispatchMessage(&msg); //window procedureにメッセージを送る
		}
		Sleep(1);
	}
	timeEndPeriod(1); //タイマーの最小精度を戻す
	return (INT)msg.wParam;
}

// ウィンドプロシージャ関数
// 受信メッセージの対応(メッセージ対象のウィンドウ,メッセージ内容,メッセージの詳細,メッセージの詳細)
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