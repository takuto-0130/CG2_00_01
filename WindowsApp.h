#pragma once
#include <Windows.h>
#include <cstdint>
class WindowsApp
{
public:// 静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:// 動的メンバ関数
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// ウィンドウハンドルのgetter
	HWND GetHwnd() const { return hwnd; }

	// ウィンドウクラスのgetter
	HINSTANCE GetHInstance() const { return wc.hInstance; }

public:// 定数
	// クライアント領域 縦横幅
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHieght = 720;
private:
	// ウィンドウハンドル
	HWND hwnd = nullptr;
	// ウィンドウクラスの設定
	WNDCLASS wc{};
};

