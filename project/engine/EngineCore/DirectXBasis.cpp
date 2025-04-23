#include "DirectXBasis.h"
#include <cassert>
#include <format>
#include <vector>
#include <thread>
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


using namespace Microsoft::WRL;
using namespace Logger;
using namespace StringUtility;

const uint32_t DirectXBasis::kMaxSRVCount_ = 512;

void DirectXBasis::Initialize(WindowsApp* windowsApp)
{
	assert(windowsApp); // NULL検出
	windowsApp_ = windowsApp;

	InitFixFPS();

	InitDevice();
	InitCommand();
	CreateSwapChain();
	CreateDepthBuffer();
	CreateVariousDescriptorHeap();
	InitRTV();
	InitDSV();
	InitFence();
	InitViewportRect();
	InitScissorRect();
	CreateDXCCompiler();
	InitImGui();
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXBasis::GetSRVCpuDescriptorHandle(const uint32_t& index)
{
	return GetCpuDescriptorHandle(srvDescripterHeap_, descriptorSizeSRV_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXBasis::GetSRVGpuDescriptorHandle(const uint32_t& index)
{
	return  GetGpuDescriptorHandle(srvDescripterHeap_, descriptorSizeSRV_, index);
}

ComPtr<IDxcBlob> DirectXBasis::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //utf8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T", profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};
	//実際にshaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);
	//dxcが起動できないなどの致命的な状況
	assert(SUCCEEDED(hr));
	//警告・エラーでログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}

	//コンパイラ結果から実行用のバイナリ部分を取得
	ComPtr<IDxcBlob> shaderBlob;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	shaderSource->Release();
	shaderResult->Release();
	shaderError->Release();
	return shaderBlob;
}

ComPtr<ID3D12Resource> DirectXBasis::CreateBufferResource(const size_t& sizeInBytes)
{
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

ComPtr<ID3D12Resource> DirectXBasis::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	//metadataを元にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//texの幅←、高さ↓
	resourceDesc.Height = UINT(metadata.height);//
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行or配列Textureの配列数
	resourceDesc.Format = metadata.format;//tex format
	resourceDesc.SampleDesc.Count = 1;// 1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//texの次元数。普段使っているのは2次元

	//利用するHeapの設定。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;		// WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;					// プロセッサの近くに配置
	////Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));
	return resource;
}

void DirectXBasis::UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,					// 全領域へコピー 
			img->pixels,				// 元データアドレス
			UINT(img->rowPitch),		// 1ラインサイズ
			UINT(img->slicePitch)		// 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

DirectX::ScratchImage DirectXBasis::LoadTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	return mipImages;
}

void DirectXBasis::ClearDepthStencilView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


void DirectXBasis::DrawBegin()
{
	//書き込むバックバッファのインデックス
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_.Transition.pResource = backBuffers_[backBufferIndex].Get();
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier_);

	
	//描画先のRTVとDSVを設定
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle_);

	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };//RGBA
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//描画用のDiscriptorHeapの設定
	/*ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescripterHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);*/

	ClearDepthStencilView();


	commandList_->RSSetViewports(1, &viewportRect_);
	commandList_->RSSetScissorRects(1, &scissorRect_);
}

void DirectXBasis::DrawEnd()
{
	HRESULT hr = S_FALSE;

	//書き込むバックバッファのインデックス
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
			//今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	CommandListAndFence();
}

void DirectXBasis::CommandListAndFence()
{
	HRESULT hr = S_FALSE;

	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// ✅ 正しいコマンドリスト配列
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists); // ← これでOK！

	swapChain_->Present(1, 0);

	// Fenceの値を更新
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	if (fence_->GetCompletedValue() != fenceValue_) {
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}

	UpdateFixFPS();

	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DirectXBasis::InitDevice()
{
	// デバッグレイヤーをオンにする
#ifdef _DEBUG

	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // DEBUG

	// DXGIファクトリーの生成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	assert(SUCCEEDED(hr));

	// アダプターの列挙
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}

	assert(useAdapter != nullptr);


	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

	// 高い順に試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {

		// デバイス生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));

		// 成功時にブレイク
		if (SUCCEEDED(hr)) {
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");


#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-iscrashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージを抑制する
		infoQueue->PushStorageFilter(&filter);

		infoQueue->Release();
	}

#endif // _DEBUG
}

void DirectXBasis::InitCommand()
{
	HRESULT hr = S_FALSE;

	// コマンドアロケータ生成
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));

	// コマンドリスト生成
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
		nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));

	// コマンドキュー生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));
}

void DirectXBasis::CreateSwapChain()
{
	HRESULT hr = S_FALSE;
	//スワップチェーン
	swapChainDesc_.Width = WindowsApp::kClientWidth;
	swapChainDesc_.Height = WindowsApp::kClientHieght;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), windowsApp_->GetHwnd(), &swapChainDesc_, nullptr,
		nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void DirectXBasis::CreateDepthBuffer()
{
	HRESULT hr = S_FALSE;

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties = D3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT); // VRAM上に作る

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WindowsApp::kClientWidth;
	resourceDesc.Height = WindowsApp::kClientHieght;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//深度のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマットResourceと合わせる
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.DepthStencil.Stencil = 0; 

	// リソースの生成
	hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度情報を書き込む状態にしておく
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer_));
	assert(SUCCEEDED(hr));

	
}

void DirectXBasis::CreateVariousDescriptorHeap()
{
	HRESULT hr = S_FALSE;

	//DescriptorSizeの取得
	//descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	rtvDescriptorHeap_ = CreateDeacriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//srvDescripterHeap_ = CreateDeacriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true);
	dsvDescriptorHeap_ = CreateDeacriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

void DirectXBasis::InitRTV()
{
	HRESULT hr = S_FALSE;

	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (uint32_t i = 0; i < 2; ++i)
	{
		//SwapChainからResourceを引っ張ってくる
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(hr));
		// RTVハンドルを取得
		rtvHandles_[i] = GetCpuDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, i);
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc_, rtvHandles_[i]);
	}
}

void DirectXBasis::InitDSV()
{
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//format 基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2DTexture
	dsvHandle_ = GetCpuDescriptorHandle(dsvDescriptorHeap_, descriptorSizeDSV_, 0);
	//DSVHEAPの先頭にDSVを作る
	device_->CreateDepthStencilView(depthBuffer_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXBasis::InitFence()
{
	HRESULT hr = S_FALSE;

	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXBasis::InitViewportRect()
{
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewportRect_.Width = WindowsApp::kClientWidth;
	viewportRect_.Height = WindowsApp::kClientHieght;
	viewportRect_.TopLeftX = 0;
	viewportRect_.TopLeftY = 0;
	viewportRect_.MinDepth = 0.0f;
	viewportRect_.MaxDepth = 1.0f;
}

void DirectXBasis::InitScissorRect()
{
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WindowsApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WindowsApp::kClientHieght;
}

void DirectXBasis::CreateDXCCompiler()
{
	HRESULT hr = S_FALSE;

	//dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void DirectXBasis::InitImGui()
{
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowsApp_->GetHwnd());
	ImGui_ImplDX12_Init(device_.Get(),
		swapChainDesc_.BufferCount, rtvDesc_.Format, srvDescripterHeap_.Get(),
		srvDescripterHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescripterHeap_->GetGPUDescriptorHandleForHeapStart());*/
}

void DirectXBasis::InitFixFPS()
{
	// 現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXBasis::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒 (よりわずかに短い時間) 経っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}


ComPtr<ID3D12DescriptorHeap> DirectXBasis::CreateDeacriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, const bool& shaderVisible)
{
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXBasis::GetCpuDescriptorHandle(const ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXBasis::GetGpuDescriptorHandle(const ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
