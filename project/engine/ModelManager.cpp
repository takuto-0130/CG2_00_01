#include "ModelManager.h"

std::unique_ptr<ModelManager> ModelManager::instance = nullptr;
std::once_flag ModelManager::initInstanceFlag;

ModelManager* ModelManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = std::make_unique<ModelManager>();
		});
	return instance.get();
}

void ModelManager::Initialize(DirectXBasis* dxManager, SrvManager* srvManager)
{
	modelLoader_ = std::make_unique<ModelLoader>();
	modelLoader_->Initialize(dxManager, srvManager);
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& fileName)
{
	if (models_.contains(fileName)) {
		return;// 読み込み済みなら早期return
	}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelLoader_.get(), directoryPath, fileName);

	models_.insert(std::make_pair(fileName, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	if (models_.contains(filePath)) {
		// 読み込みモデルを戻り値としてreturn
		return models_.at(filePath).get();
	}

	// ファイル名が一致しなければnull
	return nullptr;
}