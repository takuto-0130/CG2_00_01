#pragma once
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>
#include <XInput.h>
#include <Windows.h>
#include <array>
#include <vector>
#include <wrl.h>
#include <mutex>

#include "struct.h"

/// <summary>
/// 入力
/// </summary>
class Input {

public:
	struct MouseMove {
		LONG lX;
		LONG lY;
		LONG lZ;
	};

public:
	enum class PadType {
		DirectInput,
		XInput,
	};

	// variantがC++17から
	union State {
		XINPUT_STATE xInput_;
		DIJOYSTATE2 directInput_;
	};

	struct Joystick {
		Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
		int32_t deadZoneL_;
		int32_t deadZoneR_;
		PadType type_;
		State state_;
		State statePre_;
	};


private:
	static Input* instance;
	static std::once_flag initInstanceFlag;

	Input() = default;
	~Input() = default;
	Input(Input&) = default;
	Input& operator=(Input&) = default;
public: // メンバ関数
	static Input* GetInstance();

	// 初期化
	void Initialize(const HWND& hwnd);

	// 終了
	void Finalize();

	// 毎フレーム処理
	void Update();

	// キーの押下
	bool PushKey(BYTE keyNumber) const;

	// キーのトリガー
	bool TriggerKey(BYTE keyNumber) const;

	// 全キー情報取得
	const std::array<BYTE, 256>& GetAllKey() const { return key_; }

	// 全マウス情報取得
	const DIMOUSESTATE2& GetAllMouse() const;

	/// <summary>
	/// マウスの押下
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	bool IsPressMouse(int32_t mouseNumber) const;

	/// <summary>
	/// マウスのトリガー
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	bool IsTriggerMouse(int32_t buttonNumber) const;

	/// マウス移動量
	MouseMove GetMouseMove();

	/// ホイールスクロール量
	int32_t GetWheel() const;

	/// マウスのウィンドウ座標
	const Vector2& GetMousePosition() const;

	/// <summary>
	/// 現在のジョイスティック状態
	/// </summary>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickState(int32_t stickNo, DIJOYSTATE2& out) const;

	/// <summary>
	/// 前回のジョイスティック状態
	/// </summary>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickStatePrevious(int32_t stickNo, DIJOYSTATE2& out) const;

	/// <summary>
	/// 現在のジョイスティック状態
	/// </summary>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// 前回のジョイスティック状態
	/// </summary>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// デッドゾーンを設定
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="deadZone">デッドゾーン 0~32768</param>
	void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

	/// 接続されているジョイスティック数
	size_t GetNumberOfJoysticks();

private:
	static BOOL CALLBACK
		EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept;
	
private: // メンバ変数
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devKeyboard_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
	std::vector<Joystick> devJoysticks_;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 mousePre_;
	HWND hwnd_;
	HWND cliantHwnd_;
	Vector2 mousePosition_;
};
