#pragma once
#include "DirectXBasis.h"
#include "SrvManager.h"
class ModelLoader
{
public: // メンバ関数
	void Initialize(DirectXBasis* dxBasis, SrvManager* srvManager);

public:
	DirectXBasis* GetDirectXBasis() const { return dxBasis_; }
	SrvManager* GetSrvManager() const { return srvManager_; }

private:
	DirectXBasis* dxBasis_ = nullptr;
	SrvManager* srvManager_ = nullptr;
};

