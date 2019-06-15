// GeometryInstancingTest2.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "resource.h"

#include <Windows.h>
#include "DXManager.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <iostream>
#include <sstream>
#include "FPSCountor.h"

#define WIN_STYLE WS_OVERLAPPEDWINDOW

using namespace MyDirectX;

//ウィンドウクラス作成に必要な情報群
HWND mWindowHandle;
std::string mClassName = _T("main_window");
//表示されるウィンドウの名前
std::string mWindowName = _T("簡単お手軽最小構成(多分)");
//DirectX管理
std::unique_ptr<DXManager> mDXManager;
#if _DEBUG
std::unique_ptr<FPSCountor> mFPSCounter;
#endif
//コールバック
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
//1フレームの時間
const float FRAME_TIME = 1.0f / 60.0f;
//フレームの経過時間
float frameTime = 0;
//計測開始時間
LARGE_INTEGER timeStart;
//計測終了時間
LARGE_INTEGER timeEnd;
//計測周波数
LARGE_INTEGER timeFreq;

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
	//周波数取得
	QueryPerformanceFrequency(&timeFreq);
	//計測開始時間の初期化
	QueryPerformanceCounter(&timeStart);
	//何フレーム目か
	int frameCount = 0;
	//ログを出力する回数
	int outputLogCount = 10;
	float fps = 0.0f;
	while (msg.message != WM_QUIT) 
	{
		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			
			// 今の時間を取得
			QueryPerformanceCounter(&timeEnd);
			// (今の時間 - 前フレームの時間) / 周波数 = 経過時間(秒単位)
			frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

			//経過時間が1/60秒未満(処理時間に余裕がある)
			if (frameTime < FRAME_TIME) 
			{ 
				//Sleepの時間を計算
				DWORD sleepTime = static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
				//分解能を上げる(こうしないとSleepの精度はガタガタ)
				timeBeginPeriod(1); 
				//寝る
				Sleep(sleepTime);  
				//戻す
				timeEndPeriod(1);   
				//処理を中断しループへ
				continue;
			}
			else
			{ 
				frameCount++;
				bool isOutputLog = frameCount % outputLogCount == 0;
				fps = mFPSCounter->GetFPS();
#ifdef _DEBUG
#ifdef UNICODE
				std::wstringstream stream;
#else
				std::stringstream stream;
#endif
				if(isOutputLog)
				{
					stream << fps << " FPS" << std::endl;
					OutputDebugString(stream.str().c_str());
				}
#endif
			}
			//計測終了時間を計測開始時間に
			timeStart = timeEnd; 
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
#if _DEBUG
		mFPSCounter = std::make_unique<FPSCountor>();
#endif
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
