#include "Input.h"
#include <stdexcept>
#include <assert.h>

Input* Input::instance = nullptr;
std::once_flag Input::initInstanceFlag;

Input* Input::GetInstance()
{
    std::call_once(initInstanceFlag, []() {
        instance = new Input();
        });
    return instance;
}

// 初期化処理
void Input::Initialize(const HWND& hwnd) {
    HRESULT hr;
    cliantHwnd_ = hwnd;
    hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput_, nullptr);
    assert(SUCCEEDED(hr));

    hr = dInput_->CreateDevice(GUID_SysKeyboard, &devKeyboard_, nullptr);
    assert(SUCCEEDED(hr));

    hr = dInput_->CreateDevice(GUID_SysMouse, &devMouse_, nullptr);
    assert(SUCCEEDED(hr));

    devKeyboard_->SetDataFormat(&c_dfDIKeyboard);
    devKeyboard_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    devKeyboard_->Acquire();

    devMouse_->SetDataFormat(&c_dfDIMouse2);
    devMouse_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    devMouse_->Acquire();

    dInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
}

void Input::Finalize()
{
    if (devKeyboard_) {
        devKeyboard_->Unacquire();
    }
    if (devMouse_) {
        devMouse_->Unacquire();
    }
}

void Input::Update() {
    // 前回のキーとマウスの状態を保存
    keyPre_ = key_;
    mousePre_ = mouse_;

    devKeyboard_->GetDeviceState(sizeof(key_), key_.data());

    devMouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouse_);

    for (auto& joystick : devJoysticks_) {
        joystick.statePre_ = joystick.state_;

        if (joystick.type_ == PadType::XInput) {
            XInputGetState(0, &joystick.state_.xInput_);
        }
        else {
            joystick.device_->Poll();
            joystick.device_->GetDeviceState(sizeof(DIJOYSTATE2), &joystick.state_.directInput_);
        }
    }
}

bool Input::PushKey(BYTE keyNumber) const {
    return key_[keyNumber] & 0x80;
}

bool Input::TriggerKey(BYTE keyNumber) const {
    return (key_[keyNumber] & 0x80) && !(keyPre_[keyNumber] & 0x80);
}

const DIMOUSESTATE2& Input::GetAllMouse() const {
    return mouse_;
}

bool Input::IsPressMouse(int32_t buttonNumber) const {
    return mouse_.rgbButtons[buttonNumber] & 0x80;
}

bool Input::IsTriggerMouse(int32_t buttonNumber) const {
    return (mouse_.rgbButtons[buttonNumber] & 0x80) && !(mousePre_.rgbButtons[buttonNumber] & 0x80);
}

Input::MouseMove Input::GetMouseMove() {
    MouseMove move = { mouse_.lX, mouse_.lY, mouse_.lZ };
    return move;
}

int32_t Input::GetWheel() const {
    return mouse_.lZ;
}

const Vector2& Input::GetMousePosition() const {
    POINT point;
    if (GetCursorPos(&point)) {
        if (cliantHwnd_) {
            ScreenToClient(cliantHwnd_, &point);
           
        }
    }
    return { static_cast<float>(point.x),static_cast<float>(point.y) };
}

bool Input::GetJoystickState(int32_t stickNo, DIJOYSTATE2& out) const {
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].state_.directInput_;
    return true;
}

bool Input::GetJoystickStatePrevious(int32_t stickNo, DIJOYSTATE2& out) const {
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].statePre_.directInput_;
    return true;
}

bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const {
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].state_.xInput_;
    return true;
}

bool Input::GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const {
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].statePre_.xInput_;
    return true;
}

void Input::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR) {
    if (stickNo >= devJoysticks_.size()) return;
    devJoysticks_[stickNo].deadZoneL_ = deadZoneL;
    devJoysticks_[stickNo].deadZoneR_ = deadZoneR;
}

// 接続されているジョイスティックの数を取得
size_t Input::GetNumberOfJoysticks() {
    return devJoysticks_.size();
}

BOOL CALLBACK Input::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept {
    auto input = static_cast<Input*>(pContext);
    Microsoft::WRL::ComPtr<IDirectInputDevice8> joystick;

    if (FAILED(input->dInput_->CreateDevice(pdidInstance->guidInstance, &joystick, nullptr))) {
        return DIENUM_CONTINUE;
    }

    Joystick newJoystick = {};
    newJoystick.device_ = joystick;
    newJoystick.type_ = PadType::DirectInput;
    input->devJoysticks_.push_back(newJoystick);

    return DIENUM_CONTINUE;
}
