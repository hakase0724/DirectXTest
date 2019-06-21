//http://princess-tiara.biz/directx/?chapter=13 <-参考サイト
#include "stdafx.h"
#include "DXInput.h"
//未解決エラー吐かれるから定義してまとめたもの
#include "dinputex.h"

#pragma comment(lib , "dinput8.lib")
//GUID系使うのに必要だった
#pragma comment(lib, "dxguid.lib")
//無いとエラー吐かれるんだって
#define INITGUID

using namespace MyDirectX;

DXInput::DXInput(HWND hwnd)
{
	InitDirectInput(hwnd);
}

DXInput::~DXInput()
{
	ExitDirectInput();
}

HRESULT DXInput::InitDirectInput(HWND hwnd)
{
	HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	HRESULT hr = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mInput, NULL);
	if (FAILED(hr)) return S_FALSE;
	hr = mInput->CreateDevice(GUID_SysKeyboard, &mInputDevice, NULL);
	if (FAILED(hr))
	{
		ExitDirectInput();
		return S_FALSE;
	}
	hr = mInputDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		ExitDirectInput();
		return S_FALSE;
	}
	//入力の取り方　前面にいるときのみ取得し入力を独占しない
	hr = mInputDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		ExitDirectInput();
		return S_FALSE;
	}
	//入力受付
	mInputDevice->Acquire();
	return S_OK;
}

void DXInput::SetInputState()
{
	if(FAILED(mInputDevice->GetDeviceState(sizeof(mInputBuffer), (LPVOID)&mInputBuffer)))
	{
		//入力受付
		mInputDevice->Acquire();
	}
}

void DXInput::SetPreBuffer()
{
	for(int i = 0;i < 256;i++)
	{
		mPreInputBuffer[i] = mInputBuffer[i];
	}
}

BOOL DXInput::GetKey(int key)
{
	if (mInputBuffer[key] & isInputNum) return TRUE;
	else return FALSE;
}

BOOL DXInput::GetKeyDown(int key)
{
	if(mInputBuffer[key] & isInputNum)
	{
		if(!(mPreInputBuffer[key] & isInputNum))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL DXInput::GetKeyUp(int key)
{
	if (!(mInputBuffer[key] & isInputNum))
	{
		if (mPreInputBuffer[key] & isInputNum)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void DXInput::ExitDirectInput()
{
	//入力拒否
	mInputDevice->Unacquire();
	if (mInput) mInput->Release();
	if (mInputDevice)mInputDevice->Release();
}

