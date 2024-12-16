#include "ParticleClass.h"
#include "TextureManager.h"
#include "operatorOverload.h"
#include <numbers>

ParticleClass::Particle ParticleClass::MakeNewParticle(std::mt19937& random, const Vector3& translate) {
	Particle parti;

	std::uniform_real_distribution<float> distVec(-1.0f, 1.0f);
	parti.transform.scale = { 1.f,1.f,1.f };
	parti.transform.rotate = { 0.f,0.f,0.f };
	parti.transform.translate = { distVec(random),distVec(random),distVec(random) };
	parti.transform.translate += translate;
	parti.velocity = { distVec(random),distVec(random),distVec(random) };

	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	parti.color = { distColor(random),distColor(random),distColor(random),1.0f };

	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	parti.lifeTime = distTime(random);
	parti.currentTime = 0;

	return parti;
}

std::list<ParticleClass::Particle> ParticleClass::Emit(const Emitter& emitter, std::mt19937& random) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(random, emitter.transform.translate));
	}
	return particles;
}

bool ParticleClass::IsCollision(const AABB& a, const Vector3& point) {
	Vector3 closestPoint = { std::clamp(point.x,a.min.x,a.max.x), std::clamp(point.y,a.min.y,a.max.y), std::clamp(point.z,a.min.z,a.max.z) };
	float distance = Length(closestPoint - point);
	if (distance <= 0) {
		return true;
	}
	return false;
}


void ParticleClass::Initialize(DirectXBasis* dxBasis, SrvManager* srvManager)
{
	dxBasis_ = dxBasis;
	srvManager_ = srvManager;

	// PSO関連
	CreateRootSignature();
	inputLayoutDesc_ = CreateInputElementDesc();
	CreateBlendState();
	CreateRasterizerState();
	LoadShader();
	CreatePipelineState();
	// リソースの生成と値の設定
	CreateParticleResource();
	CreateMaterialResource();

	emitter_.transform.scale = { 1,1,1 };
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	emitter_.count = 3;

	accel.acceleration = { 5.0f,0.0f,0.0f };
	accel.area.min = { -1.0f, -1.0f, -1.0f };
	accel.area.max = { 1.0f, 1.0f, 1.0f };

	isAccel = false;

	useBillboard = true;

}

void ParticleClass::Update()
{
	std::mt19937 random(seedGene());

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorldMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	numInstance = 0;

	emitter_.frequencyTime += kDeltaTime;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		particles.splice(particles.end(), Emit(emitter_, random));
		emitter_.frequencyTime -= emitter_.frequency;
	}

	for (std::list<Particle>::iterator partiIterator = particles.begin(); partiIterator != particles.end();) {
		if ((*partiIterator).lifeTime <= (*partiIterator).currentTime) {
			partiIterator = particles.erase(partiIterator);
			continue;
		}

		if (isAccel) {
			if (IsCollision(accel.area, (*partiIterator).transform.translate)) {
				(*partiIterator).velocity = accel.acceleration;
			}
		}


		(*partiIterator).transform.rotate = transform.rotate;
		(*partiIterator).transform.translate += (*partiIterator).velocity * kDeltaTime;
		(*partiIterator).currentTime += kDeltaTime; // 経過時間を足す
		if (numInstance < kNumMaxInstance)
		{
			Matrix4x4 worldMatrixP = MakeAffineMatrix((*partiIterator).transform.scale, (*partiIterator).transform.rotate, (*partiIterator).transform.translate);
			if (useBillboard) {
				worldMatrixP = worldMatrixP * billboardMatrix;
			}
			Matrix4x4 WVPMatrix = worldMatrixP * camera_->GetViewProjectionMatrix();
			instancingData_[numInstance].WVP = WVPMatrix;
			instancingData_[numInstance].World = worldMatrixP;
			instancingData_[numInstance].color = (*partiIterator).color;
			float alpha = 1.0f - ((*partiIterator).currentTime / (*partiIterator).lifeTime);
			instancingData_[numInstance].color.w = alpha;
			++numInstance; // 生きてるパーティクルをカウント
		}
		++partiIterator;
	}

}

void ParticleClass::Draw()
{
	auto commandList = dxBasis_->GetCommandList();
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(graphicsPipelineState_.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	srvManager_->SetGraphicsRootDescriptorTable(1, srvIndex);
	srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath));
	commandList->DrawInstanced(UINT(modelData.vertices.size()), numInstance, 0, 0);
}

void ParticleClass::CreateRootSignature()
{
	// Particle用のRootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;		// 0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;			// 数は1つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;														// 0から始まる
	descriptorRange[0].NumDescriptors = 1;															// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;									// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertezShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;			// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;							// レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;			// Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);		// Tableで利用する数

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			// DescriptorTableで使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;					// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);		// Tableで利用する数								// レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;						// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);			// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリをもとに生成

	hr = dxBasis_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

D3D12_INPUT_LAYOUT_DESC ParticleClass::CreateInputElementDesc()
{
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	return inputLayoutDesc;
}

void ParticleClass::CreateBlendState()
{
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void ParticleClass::CreateRasterizerState()
{
	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void ParticleClass::LoadShader()
{
	// Shaderをコンパイルする
	vertexShaderBlob_ = dxBasis_->CompileShader(L"Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxBasis_->CompileShader(L"Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void ParticleClass::CreatePipelineState()
{
	HRESULT hr;

	// PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();			// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;			// InputLayout
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };			// vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };			// PixelShade
	graphicsPipelineStateDesc.BlendState = blendDesc_;					// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;			// RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	hr = dxBasis_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void ParticleClass::CreateParticleResource()
{
	// インスタンス用のTransformationMatrixリソースを作る
	instancingResource_ = dxBasis_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// 単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {

		instancingData_[index].WVP = MakeIdentity4x4();

		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// モデルの読み込み
	modelData.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData.material.textureFilePath = "Resources/circle.png";

	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	srvIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);

	// 頂点リソースを作る
	vertexResource = dxBasis_->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	vertexBufferView_.BufferLocation = vertexResource->GetGPUVirtualAddress();	// リソースの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());		// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);	// 1頂点当たりのサイズ

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));		// 書き込むためのアドレスを取得
	std::memcpy(vertexData_, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	srvManager_->CreateSRVforStructuredBuffer(srvIndex, instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));
}

void ParticleClass::CreateMaterialResource()
{
	// マテリアル用のリソースを作る。
	materialResource_ = dxBasis_->CreateBufferResource(sizeof(Material));
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 白を入れる
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
}
