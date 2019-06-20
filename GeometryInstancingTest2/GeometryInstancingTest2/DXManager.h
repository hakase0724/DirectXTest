#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
//ウィンドウの横幅
static const UINT WindowWidth = 1000;
//ウィンドウの縦幅
static const UINT WindowHeight = 800;

// 一つの頂点情報を格納する構造体
struct VERTEX 
{
	DirectX::XMFLOAT3 V;
};

// GPU(シェーダ側)へ送る数値をまとめた構造体
struct CONSTANT_BUFFER 
{
	DirectX::XMMATRIX mWVPs[10];
};

struct PerInstanceData
{
	DirectX::XMMATRIX matrix;
};

namespace MyDirectX
{
	class DXManager
	{
	public:
		DXManager(HWND hwnd);
		void Update();
		~DXManager();
	private:
		//DirectXに必要な変数
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<ID3D11RasterizerState> mRasterizerState;
		ComPtr<ID3D11VertexShader> mVertexShader;
		ComPtr<ID3D11PixelShader> mPixelShader;
		ComPtr<ID3D11InputLayout> mInputLayout;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		ComPtr<ID3D11Buffer> mConstantBuffer;
		ComPtr<ID3D11Buffer> mVertexBuffer;
		ComPtr<ID3D11Buffer> mIndexBuffer;
		ComPtr<ID3D11Buffer> mPerInstanceBuffer;
		ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
		//描画する頂点数
		int mDrawNum;
		int mInstanceNum;
	};
}
