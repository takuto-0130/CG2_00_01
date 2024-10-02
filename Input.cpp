#include "Input.h"
#include <assert.h>

void Input::Initialize(WindowsApp* windowsApp)
{
	windowsApp_ = windowsApp;
	// DirectInputを初期化
	HRESULT hr = DirectInput8Create(windowsApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));
	// キーボードの初期化
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	hr = keyboard_->SetCooperativeLevel(
		windowsApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));
}

void Input::Update()
{
	HRESULT hr;
	memcpy(preKey_, key_, sizeof(key_));
	hr = keyboard_->Acquire();
	hr = keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool Input::PushKey(BYTE keyNum)
{
	// 押していればtrue
	if (key_[keyNum]) {
		return true;
	}
	// それ以外でfalse
	return false;
}

bool Input::TriggerKey(BYTE keyNum)
{
	// トリガーしたときtrue
	if (key_[keyNum] && !preKey_[keyNum]) {
		return true;
	}
	// それ以外でfalse
	return false;
}
