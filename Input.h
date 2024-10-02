#pragma once
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>
#include <wrl.h>
#pragma comment(lib, "dinput8.lib")
class Input
{
public:
	// namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // メンバ関数
	// 初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);

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

private:

	ComPtr<IDirectInputDevice8> keyboard = nullptr;
	ComPtr<IDirectInput8> directInput = nullptr;
	BYTE key[256] = {};
	BYTE preKey[256] = {};
};

