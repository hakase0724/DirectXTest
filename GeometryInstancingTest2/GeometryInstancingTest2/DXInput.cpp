//http://princess-tiara.biz/directx/?chapter=13 <-�Q�l�T�C�g
#include "stdafx.h"
#include "DXInput.h"
//�������G���[�f����邩���`���Ă܂Ƃ߂�����
#include "dinputex.h"

#pragma comment(lib , "dinput8.lib")
//GUID�n�g���̂ɕK�v������
#pragma comment(lib, "dxguid.lib")
//�����ƃG���[�f�����񂾂���
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
	//���͂̎����@�O�ʂɂ���Ƃ��̂ݎ擾�����͂�Ɛ肵�Ȃ�
	hr = mInputDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		ExitDirectInput();
		return S_FALSE;
	}
	//���͎�t
	mInputDevice->Acquire();
	return S_OK;
}

void DXInput::SetInputState()
{
	if(FAILED(mInputDevice->GetDeviceState(sizeof(mInputBuffer), (LPVOID)&mInputBuffer)))
	{
		//���͎�t
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
	//���͋���
	mInputDevice->Unacquire();
	if (mInput) mInput->Release();
	if (mInputDevice)mInputDevice->Release();
}

