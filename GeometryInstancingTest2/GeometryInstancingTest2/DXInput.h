#pragma once
#include "dinput.h"

namespace MyDirectX
{
	class DXInput
	{
	public:
		DXInput(HWND hwnd);
		~DXInput();
		HRESULT InitDirectInput(HWND hwnd);
		void SetInputState();
		//1�t���[���O�̓��͏����i�[
		void SetPreBuffer();
		//�����Ă���
		BOOL GetKey(int key);
		//�����ꂽ�u��
		BOOL GetKeyDown(int key);
		//�����ꂽ�u��
		BOOL GetKeyUp(int key);
		void ExitDirectInput();
	private:
		//���̒l�Ƃ̘_���ς��Ƃ�Ɠ��͂��Ƃ��
		const int isInputNum = 0x80;
		LPDIRECTINPUT8 mInput;
		LPDIRECTINPUTDEVICE8 mInputDevice;
		char mInputBuffer[256];
		char mPreInputBuffer[256];
	};
}

