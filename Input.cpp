#include "Input.h"
#include <assert.h>

void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{

	// DirectInputを初期化
	HRESULT hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));
	// キーボードの初期化
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	hr = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));
}

void Input::Update()
{
	HRESULT hr;
	memcpy(preKey, key, sizeof(key));
	hr = keyboard->Acquire();
	hr = keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNum)
{
	// 押していればtrue
	if (key[keyNum]) {
		return true;
	}
	// それ以外でfalse
	return false;
}

bool Input::TriggerKey(BYTE keyNum)
{
	// トリガーしたときtrue
	if (key[keyNum] && !preKey[keyNum]) {
		return true;
	}
	// それ以外でfalse
	return false;
}
