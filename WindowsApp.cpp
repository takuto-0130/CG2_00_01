//#include "WindowsApp.h"
//#include <Windows.h>
//#include <cstdint>
//#include "externals/imgui/imgui_impl_dx12.h"
//#include "externals/imgui/imgui.h"
//#include "externals/imgui/imgui_impl_win32.h"
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//
//
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
//
//	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
//		return true;
//	}
//
//	switch (msg) {
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		return 0;
//	}
//
//	return DefWindowProc(hwnd, msg, wparam, lparam);
//}
//
//void WindowsApp::Initialize()
//{
//	CoInitializeEx(0, COINIT_MULTITHREADED);
//
//	//
//	WNDCLASS wc{};
//	wc.lpfnWndProc = WindowProc;
//
//	wc.lpszClassName = L"CG2WindowClass";
//
//	wc.hInstance = GetModuleHandle(nullptr);
//
//	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//
//	RegisterClass(&wc);
//	//
//
//	//
//	const int32_t kClientWidth = 1280;
//	const int32_t kClientHieght = 720;
//
//	RECT wrc = { 0, 0, kClientWidth, kClientHieght };
//
//	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
//	//
//
//
//
//	//
//	HWND hwnd = CreateWindow(
//		wc.lpszClassName,
//		L"CG2",
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT,
//		CW_USEDEFAULT,
//		wrc.right - wrc.left,
//		wrc.bottom - wrc.top,
//		nullptr,
//		nullptr,
//		wc.hInstance,
//		nullptr);
//
//	ShowWindow(hwnd, SW_SHOW);
//}
//
//void WindowsApp::Update()
//{
//}
