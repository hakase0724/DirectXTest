#pragma once
#include <d3d11.h>
#include <string>
#include <wrl/client.h>
#pragma comment(lib, "d3d11.lib")

using namespace Microsoft::WRL;
//ウィンドウの横幅
static const UINT WindowWidth = 1000;
//ウィンドウの縦幅
static const UINT WindowHeight = 800;

namespace MyDirectX
{
	class DXManager
	{
	public:
		DXManager(HWND hwnd);
		void Update();
		~DXManager() {};
	private:
		//DirectXに必要な変数
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		float *mClearColor;
	};
}
