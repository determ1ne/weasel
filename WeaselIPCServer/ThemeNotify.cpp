#include "stdafx.h"
#include "ThemeNotify.h"
#include "WeaselServerImpl.h"
#include "resource.h"

constexpr WCHAR szTNWindowClass[] = L"ThemeNotifyWindowClass";
constexpr WCHAR themeRegKey[] = LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)";

BOOL _tnInited = FALSE;
DWORD SysThemeIsLight = 0;
DWORD AppThemeIsLight = 0;
weasel::ServerImpl* _server;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL InitThemeNotifier(weasel::ServerImpl* server)
{
	if (_tnInited) return TRUE;
	_tnInited = TRUE;
	_server = server;

	HINSTANCE hInst = GetModuleHandle(NULL);

	// register window class
	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInst;
	wcex.lpszClassName = szTNWindowClass;

	ATOM r = RegisterClassExW(&wcex);
	if (r == NULL) return FALSE;

	HWND hWnd = CreateWindowW(szTNWindowClass, L"", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
	if (!hWnd) return FALSE;

	return TRUE;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message != WM_SETTINGCHANGE) return DefWindowProc(hWnd, message, wParam, lParam);

	if (auto lParamString = reinterpret_cast<const wchar_t*>(lParam))
	{
		if (!wcscmp(lParamString, L"ImmersiveColorSet"))
		{
			ReadColorMode();
		}
	}

	return 0;
}

void ReadColorMode()
{
	HKEY hKey;
	DWORD dwBuffer;
	DWORD dwDataSize;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, themeRegKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return;

	dwDataSize = sizeof(dwBuffer);
	if (RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, NULL, (LPBYTE) &dwBuffer, &dwDataSize) == ERROR_SUCCESS)
	{
		if (dwBuffer != SysThemeIsLight)
		{
			BOOL handled = TRUE;
			SysThemeIsLight = dwBuffer;
			_server->OnCommand(NULL, SysThemeIsLight ? ID_WEASELTRAY_SYS_LIGHT_MODE : ID_WEASELTRAY_SYS_DARK_MODE, NULL, handled);
		}
	}

	dwDataSize = sizeof(dwBuffer);
	if (RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&dwBuffer, &dwDataSize) == ERROR_SUCCESS)
	{
		if (dwBuffer != AppThemeIsLight)
		{
			BOOL handled = TRUE;
			AppThemeIsLight = dwBuffer;
			_server->OnCommand(NULL, AppThemeIsLight ? ID_WEASELTRAY_APP_LIGHT_MODE : ID_WEASELTRAY_APP_DARK_MODE, NULL, handled);
		}
	}

	RegCloseKey(hKey);
}

