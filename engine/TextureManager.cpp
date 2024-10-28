#include "TextureManager.h"
#include "DirectXBasis.h"


TextureManager* TextureManager::instance_ = nullptr;

TextureManager* TextureManager::GetInstance()
{
	if (instance_ == nullptr) 
	{
		instance_ = new TextureManager;
	}
	return instance_;
}

void TextureManager::Initialize()
{
	textureDatas_.reserve(DirectXBasis::kMaxSRVCount_);
}

void TextureManager::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}
