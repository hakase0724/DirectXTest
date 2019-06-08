#include "stdafx.h"
#include "DXManager.h"

using namespace MyDirectX;

DXManager::DXManager(HWND hwnd)
{
	/*�X���b�v�`�F�C���̐ݒ�*/
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
	//�`���̐ݒ�
	ID3D11Texture2D *renderTexture;
	mSwapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTexture);
	mDevice->CreateRenderTargetView(renderTexture, NULL, &mRenderTargetView);
	renderTexture->Release();
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mClearColor = clearColor;

	// �r���[�|�[�g�̐ݒ�
	mViewPort.Width = WindowWidth;
	mViewPort.Height = WindowHeight;
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;
	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;

	//���_���
	VERTEX vertex[] =
	{
		DirectX::XMFLOAT3(-0.2f, 0.5f, 0.0f),
		DirectX::XMFLOAT3(0.2f, 0.5f, 0.0f),
		DirectX::XMFLOAT3(-0.0f, 0.3f, 0.0f)
	};
	//�z��̗v�f���v�Z
	mDrawNum = sizeof(vertex) / sizeof(vertex[0]);

	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(VERTEX) * mDrawNum;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertex;
	mDevice->CreateBuffer(&bd, &InitData, &mVertexBuffer);

	//���X�^���C�U�ݒ�
	D3D11_RASTERIZER_DESC rd;
	//�h��Ԃ��ݒ�@SOLID�͓h��Ԃ�
	rd.FillMode = D3D11_FILL_SOLID;
	//�`��ʐݒ�@���͗��ʕ`��
	rd.CullMode = D3D11_CULL_NONE;
	//�\�ʐݒ�@�����v��肷��ƕ\�ƔF���@FALSE���Ƌt�]����
	rd.FrontCounterClockwise = TRUE;
	mDevice->CreateRasterizerState(&rd, &mRasterizerState);

	ID3DBlob* compileVS = NULL;
	ID3DBlob* compilePS = NULL;
	if (FAILED(D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "VS", "vs_5_0", 0, 0, &compileVS, NULL))) return;
	if (FAILED(D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &compilePS, NULL))) return;
	mDevice->CreateVertexShader(compileVS->GetBufferPointer(), compileVS->GetBufferSize(), NULL, &mVertexShader);
	mDevice->CreatePixelShader(compilePS->GetBufferPointer(), compilePS->GetBufferSize(), NULL, &mPixelShader);

	// ���_���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	mDevice->CreateInputLayout(layout, 1, compileVS->GetBufferPointer(), compileVS->GetBufferSize(), &mInputLayout);
	compileVS->Release();
	compilePS->Release();

	// �萔�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	mDevice->CreateBuffer(&cb, NULL, &mConstantBuffer2);
}

void DXManager::Update()
{
	// DirectX�̃��[�v����                        
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), mClearColor);
	// �p�����[�^�̌v�Z
	DirectX::XMVECTOR eye_pos = DirectX::XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);        // ���_�ʒu
	DirectX::XMVECTOR eye_lookat = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);      // ���_����
	DirectX::XMVECTOR eye_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);          // ���_�����
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(eye_pos, eye_lookat, eye_up);  // ������W�n�̃r���[�s��
	DirectX::XMMATRIX Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, WindowWidth / WindowHeight, 0.1f, 100.0f);  // ������W�n�̃p�[�X�y�N�e�B�u�ˉe�s��
	DirectX::XMMATRIX World = DirectX::XMMatrixRotationZ(0);

	CONSTANT_BUFFER cb;
	cb.mWVP = DirectX::XMMatrixTranspose(World * View * Proj);                               // ���\�[�X�֑���l���Z�b�g
	mDeviceContext->UpdateSubresource(mConstantBuffer2, 0, NULL, &cb, 0, 0);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	mDeviceContext->IASetInputLayout(mInputLayout.Get());
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);
	mDeviceContext->RSSetViewports(1, &mViewPort);
	mDeviceContext->VSSetShader(mVertexShader.Get(), NULL, 0);
	mDeviceContext->PSSetShader(mPixelShader.Get(), NULL, 0);
	mDeviceContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	mDeviceContext->RSSetState(mRasterizerState.Get());

	mDeviceContext->Draw(mDrawNum, 0);
	mSwapChain.Get()->Present(0, 0);
}


