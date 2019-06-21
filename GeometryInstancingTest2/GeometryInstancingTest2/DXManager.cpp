#include "stdafx.h"
#include "DXManager.h"

using namespace DirectX;
using namespace MyDirectX;

DXManager::DXManager(HWND hwnd)
{
	mInput = std::make_unique<DXInput>(hwnd);
	mInstanceNum = 40 * 500;
	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferCount = 1;
	scd.BufferDesc.Width = WindowWidth;
	scd.BufferDesc.Height = WindowHeight;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &fl, 1, D3D11_SDK_VERSION, &scd, &mSwapChain, &mDevice, NULL, &mDeviceContext);

	ID3D11Texture2D *pbbTex;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbbTex);
	mDevice->CreateRenderTargetView(pbbTex, NULL, mRenderTargetView.GetAddressOf());
	pbbTex->Release();

	// ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.Width = WindowWidth;
	vp.Height = WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// シェーダの設定
	ID3DBlob *pCompileVS = NULL;
	ID3DBlob *pCompilePS = NULL;
	D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	mDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &mVertexShader);
	D3DCompileFromFile(L"shader.hlsl", NULL, NULL, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	mDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &mPixelShader);
	D3DCompileFromFile(L"shader2.hlsl", NULL, NULL, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	mDevice->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &mVertexShader2);
	D3DCompileFromFile(L"shader2.hlsl", NULL, NULL, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	mDevice->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &mPixelShader2);

	// 頂点レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	mDevice->CreateInputLayout(layout, _countof(layout), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &mInputLayout);
	pCompileVS->Release();
	pCompilePS->Release();

	//ポリゴンの頂点データの作成とそのバッファの設定
	VERTEX vertices[] = 
	{
		XMFLOAT3(-0.5f, 0.5f, 0.0f),
		XMFLOAT3(-0.5f, -0.5f, 0.0f),
		XMFLOAT3(0.5f, 0.5f, 0.0f),
		XMFLOAT3(0.5f, -0.5f, 0.0f),
	};
	mDrawNum = sizeof(vertices) / sizeof(vertices[0]);
	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(VERTEX) * mDrawNum;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	mDevice->CreateBuffer(&bd, &vertexData, &mVertexBuffer);

	// インデックスデータ用バッファの設定
	int indexes[] = 
	{
		0,2,1,
		0,3,2,
	};
	mDrawNum = sizeof(indexes) / sizeof(indexes[0]);
	D3D11_BUFFER_DESC bd_index;
	bd_index.ByteWidth = sizeof(int) * mDrawNum;
	bd_index.Usage = D3D11_USAGE_DEFAULT;
	bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd_index.CPUAccessFlags = 0;
	bd_index.MiscFlags = 0;
	bd_index.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indexes;
	mDevice->CreateBuffer(&bd_index, &indexData, &mIndexBuffer);

	//インスタンスデータ用バッファの設定
	D3D11_BUFFER_DESC bd_instance;
	bd_instance.Usage = D3D11_USAGE_DYNAMIC;
	bd_instance.ByteWidth = sizeof(PerInstanceData) * mInstanceNum;
	bd_instance.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd_instance.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd_instance.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd_instance.StructureByteStride = sizeof(PerInstanceData);
	mDevice->CreateBuffer(&bd_instance, NULL, &mPerInstanceBuffer);
	//インスタンス用のリソースビューを作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = mInstanceNum;                 
	mDevice->CreateShaderResourceView(mPerInstanceBuffer.Get(),&srvDesc,&mShaderResourceView);

	// 定数バッファの作成(パラメータ受け渡し用)
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	mDevice->CreateBuffer(&cb, NULL, &mConstantBuffer);

	// ラスタライザの設定
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.CullMode = D3D11_CULL_NONE;
	rdc.FrontCounterClockwise = TRUE;
	mDevice->CreateRasterizerState(&rdc, &mRasterizerState);

	//パイプラインの構築
	ID3D11Buffer* bufs[] = { mVertexBuffer.Get() };
	UINT stride[] = { sizeof(VERTEX) };
	UINT offset[] = { 0 };
	mDeviceContext->IASetVertexBuffers(0, sizeof(bufs) / sizeof(bufs[0]), bufs, stride, offset);
	mDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	mDeviceContext->IASetInputLayout(mInputLayout.Get());
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), NULL);               
	mDeviceContext->RSSetViewports(1, &vp);                                      
	/*mDeviceContext->VSSetShader(mVertexShader.Get(), NULL, 0);                         
	mDeviceContext->PSSetShader(mPixelShader.Get(), NULL, 0);       */                     
	mDeviceContext->RSSetState(mRasterizerState.Get());      

	// パラメータの計算
	XMVECTOR eye_pos = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR eye_lookat = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR eye_up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	mView = XMMatrixLookAtLH(eye_pos, eye_lookat, eye_up);
	mProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)WindowWidth / (FLOAT)WindowHeight, 0.1f, 110.0f);
	mScale = XMMatrixScalingFromVector(XMVectorSet(0.1f, 0.1f, 0.1f, 0.0f));
	mRotation = XMMatrixRotationX(0.0f) * XMMatrixRotationY(0.0f) * XMMatrixRotationZ(0.0f);
}

bool DXManager::Update()
{
	mInput->SetInputState();
	//画面クリア
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);

	if(mInput->GetKeyDown(DIK_SPACE))
	{
		mIsInstancing ^= 1;
	}

	if (mIsInstancing) RenderInstancing();
	else Render();

	mSwapChain->Present(0, 0);
	if (mInput->GetKeyDown(DIK_ESCAPE)) return false;
	mInput->SetPreBuffer();
	return true;
}

void DXManager::Render()
{
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeviceContext->VSSetConstantBuffers(0,1,mConstantBuffer.GetAddressOf());
	mDeviceContext->VSSetShader(mVertexShader2.Get(), NULL, 0);
	mDeviceContext->PSSetShader(mPixelShader2.Get(), NULL, 0);
	D3D11_MAPPED_SUBRESOURCE pdata;
	CONSTANT_BUFFER cb;                              
	float defaultYPos = 1.5f;
	float offset = 0.11f;
	int oneLineNum = 40;
	for (int i = 0; i < mInstanceNum; i++)
	{
		mDeviceContext->Map(mConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		//とりあえずループ変数使って移動
		float xPos = i % oneLineNum * offset - 2.0f;
		float yPos = defaultYPos - (i / oneLineNum * offset);
		XMMATRIX move = XMMatrixTranslation(xPos, yPos, 1.0f);
		//行列情報をセット
		cb.gWVP = XMMatrixTranspose(mScale * mRotation * move * mView * mProj);
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));
		mDeviceContext->Unmap(mConstantBuffer.Get(), 0);
		mDeviceContext->DrawIndexed(mDrawNum,0,0);
	}
	
}

void DXManager::RenderInstancing()
{
	mDeviceContext->VSSetShader(mVertexShader.Get(), NULL, 0);
	mDeviceContext->PSSetShader(mPixelShader.Get(), NULL, 0);
	// パラメータの受け渡し
	D3D11_MAPPED_SUBRESOURCE pdata;
	mDeviceContext->Map(mPerInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
	PerInstanceData* instanceData = (PerInstanceData*)(pdata.pData);
	float defaultYPos = 1.5f;
	float offset = 0.11f;
	int oneLineNum = 40;
	for (int i = 0; i < mInstanceNum; i++)
	{
		//とりあえずループ変数使って移動
		float xPos = i % oneLineNum * offset - 2.0f;
		float yPos = defaultYPos - (i / oneLineNum * offset);
		XMMATRIX move = XMMatrixTranslation(xPos, yPos, 1.0f);
		//行列情報をセット
		instanceData[i].matrix = XMMatrixTranspose(mScale * mRotation * move * mView * mProj);
		//色情報をセット
		instanceData[i].color = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	}
	mDeviceContext->Unmap(mPerInstanceBuffer.Get(), 0);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mDeviceContext->VSSetShaderResources(0, 1, mShaderResourceView.GetAddressOf());
	mDeviceContext->PSSetShaderResources(0, 1, mShaderResourceView.GetAddressOf());
	// 描画実行
	mDeviceContext->DrawIndexedInstanced(mDrawNum, mInstanceNum, 0, 0, 0);
}

DXManager::~DXManager(){}


