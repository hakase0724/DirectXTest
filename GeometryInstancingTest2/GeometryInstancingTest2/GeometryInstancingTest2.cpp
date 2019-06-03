// GeometryInstancingTest2.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "resource.h"

#include <Windows.h>
#include "DXManager.h"

#define WIN_STYLE WS_OVERLAPPEDWINDOW

using namespace MyDirectX;

//ウィンドウクラス作成に必要な情報群
HWND mWindowHandle;
std::string mClassName = _T("main_window");
//表示されるウィンドウの名前
std::string mWindowName = _T("簡単お手軽最小構成(多分)");
//DirectX管理
std::unique_ptr<DXManager> mDXManager;
//コールバック
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

//今回のメイン関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR pCmdLine, int nCmdShow) 
{
	// ウィンドウクラスを登録する
	WNDCLASS windowClass = { 0 };
	windowClass.lpfnWndProc = WinProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = mClassName.c_str();
	RegisterClass(&windowClass);
	// ウィンドウの作成
	mWindowHandle = CreateWindow(mClassName.c_str(), mWindowName.c_str(), WIN_STYLE, CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);
	if (mWindowHandle == NULL) return 0;
	ShowWindow(mWindowHandle, nCmdShow);
	// メッセージループの実行
	MSG msg = { 0 };
	//背景クリア色

	while (msg.message != WM_QUIT) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			// DirectXのループ処理                        
			mDXManager->Update();
		}
	}
	return 0;
}


LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
	case WM_CREATE:
	{
		//ウィンドウ生成のタイミングでDirectXの初期化を行う
		mDXManager = std::make_unique<DXManager>(hwnd);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
