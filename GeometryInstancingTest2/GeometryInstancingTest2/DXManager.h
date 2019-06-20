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
//�E�B���h�E�̉���
static const UINT WindowWidth = 1000;
//�E�B���h�E�̏c��
static const UINT WindowHeight = 800;

// ��̒��_�����i�[����\����
struct VERTEX 
{
	DirectX::XMFLOAT3 V;
};

// GPU(�V�F�[�_��)�֑��鐔�l���܂Ƃ߂��\����
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
		//DirectX�ɕK�v�ȕϐ�
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
		//�`�悷�钸�_��
		int mDrawNum;
		int mInstanceNum;
	};
}
