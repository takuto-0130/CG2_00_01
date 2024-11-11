#pragma once
#include <map>
#include <memory>
#include <string>
#include "Model.h"
#include <mutex>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <TextureManager.h>

class ModelManager
{
public:
	ModelManager() = default;
	~ModelManager() = default;
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 初期化
	void Initialize(DirectXBasis* dxManager, SrvManager* srvManager);

public:
	// モデルファイル読み込み
	void LoadModel(const std::string& directoryPath, const std::string& filePath);
	// モデルの検索
	Model* FindModel(const std::string& filePath);
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;

private:

	std::unique_ptr<ModelLoader> modelLoader_ = nullptr;

public:
	ModelLoader* GetModelLoader() { return modelLoader_.get(); }

private:
	static std::unique_ptr<ModelManager> instance;
	static std::once_flag initInstanceFlag;

	ModelManager(ModelManager&) = default;
	ModelManager& operator=(ModelManager&) = default;
};

