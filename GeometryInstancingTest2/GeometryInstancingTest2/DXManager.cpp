#include "stdafx.h"
#include "DXManager.h"

using namespace MyDirectX;

DXManager::DXManager(HWND hwnd)
{
	/*スワップチェインの設定*/
	DXGI_SWAP_CHAIN_DESC swapChain = { 0 };
	swapChain.BufferDesc.Width = WindowWidth;
	swapChain.BufferDesc.Height = WindowHeight;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.SampleDesc.Count = 1;
	swapChain.SampleDesc.Quality = 0;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 1;
	swapChain.OutputWindow = hwnd;
	swapChain.Windowed = TRUE;

	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &fl, 1, D3D11_SDK_VERSION, &swapChain, &mSwapChain, &mDevice, NULL, &mDeviceContext);
	//描画先の設定
	ID3D11Texture2D *renderTexture;
	mSwapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTexture);
	mDevice->CreateRenderTargetView(renderTexture, NULL, &mRenderTargetView);
	renderTexture->Release();
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mClearColor = clearColor;
}

void DXManager::Update()
{
	// DirectXのループ処理                        
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), mClearColor);
	mSwapChain.Get()->Present(0, 0);
}


