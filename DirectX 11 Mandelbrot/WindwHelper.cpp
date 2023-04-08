#include "WindowHelper.h"

#include "WindowHelper.h"
#include <iostream>
#include "imGui\imconfig.h"
#include "imGui\imgui.h"
#include "imGui\imgui_impl_dx11.h"
#include "imGui\imgui_internal.h"
#include "imGui\imstb_rectpack.h"
#include "imGui\imstb_textedit.h"
#include "imGui\imstb_truetype.h"
#include "imGui\imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
	const wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;

	RECT rect = {};
	rect.left = LONG(GetSystemMetrics(SM_CXSCREEN) * 0.5f - width * 0.5f);

	rect.top = LONG(GetSystemMetrics(SM_CYSCREEN) * 0.5f - height * 0.5f);

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	RegisterClass(&wc);

	window = CreateWindowEx(0, CLASS_NAME, L"Fractals", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, instance, nullptr);

	if (window == nullptr)
	{
		std::cerr << "HWND was, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(window, nCmdShow);

	return true;
}
