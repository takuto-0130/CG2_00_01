#pragma once
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>
#include <wrl.h>
#include "WindowsApp.h"

#pragma comment(lib, "dinput8.lib")
class Input
{
public: // namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // メンバ関数
	// 初期化
	void Initialize(WindowsApp* windowsApp);

	// 更新
	void Update();

	/// <summary>
	/// キーの押下を確認
	/// </summary>
	/// <returns>押されている時 : true</returns>
	bool PushKey(BYTE keyNum);

	/// <summary>
	/// キーのトリガーを確認
	/// </summary>
	/// <returns>トリガーした時 : true</returns>
	bool TriggerKey(BYTE keyNum);

private: // メンバ変数
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	ComPtr<IDirectInput8> directInput_ = nullptr;
	BYTE key_[256] = {};
	BYTE preKey_[256] = {};
	WindowsApp* windowsApp_ = nullptr;
};

