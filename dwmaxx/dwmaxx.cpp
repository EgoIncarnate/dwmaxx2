#include <Windows.h>
#include <dwmapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include "globals.h"
#include "constants.h"
#include "dwmaxx.h"
#include "dwmaxx_private.h"
#include "injection.h"
#include "win_hooks.h"
#include "watermarking.h"
#include "hooking.h"
#include "rpc_hwnd.h"

#pragma comment (lib, "dwmapi.lib")

HRESULT DwmaxxLoad()
{
    HRESULT hr = E_FAIL;
    if (DwmaxxIsLoaded() == TRUE)
        return (E_FAIL);

    BOOL    isWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isWow64);
    if (isWow64 == TRUE)
    {
        char    currentDir[MAX_PATH];
        char    systemDir[MAX_PATH];
        char    fullPath[MAX_PATH];
        PROCESS_INFORMATION pi;
        STARTUPINFO         si;
        GetModuleFileName(g_hInstance, currentDir, sizeof(fullPath));
        PathRemoveFileSpec(currentDir);
        GetSystemDirectory(systemDir, sizeof(systemDir));
        sprintf(fullPath, "%s\\rundll32.exe %s\\dwmaxx64.dll,DwmaxxInject", systemDir, currentDir);
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);
        CreateProcess(NULL, fullPath, NULL, NULL, NULL, NULL, NULL, NULL, &si, &pi);
        WaitForSingleObject(pi.hProcess, INFINITE);
        if (DwmaxxIsLoaded() == FALSE)
            hr = E_FAIL;
        else
            hr = S_OK;
    }
    else
        hr = DwmaxxInject();

    if (FAILED(hr))
        return (hr);

    // Now restart composition to get a new D3D device
    DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
    DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);

    return (hr);
}

HRESULT DwmaxxUnload()
{
    if (DwmaxxIsLoaded() == FALSE)
        return (E_FAIL);
    HWND hw = DwmaxxRpcWindow();
    SendMessage(DwmaxxRpcWindow(), DWMAXX_UNLOAD, NULL, NULL);
    return (S_OK);
}

BOOL    DwmaxxIsLoaded()
{
    BOOL    isWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isWow64);
    if (isWow64 == TRUE)
        return (DwmaxxRpcWindow() != NULL);
    return (DwmaxxIsInjected());
}

HANDLE  DwmaxxGetWindowSharedHandle(HWND hWnd)
{
    return (HANDLE)SendMessage(DwmaxxRpcWindow(), DWMAXX_GET_SHARED_HANDLE, (WPARAM)hWnd, NULL);
}

void    DwmaxxGetWindowSharedHandleAsync(HWND hWnd, HWND callbackHwnd)
{
    PostMessage(DwmaxxRpcWindow(), DWMAXX_GET_SHARED_HANDLE, (WPARAM)hWnd, (LPARAM)callbackHwnd);
}

void    DwmaxxRemoveWindow(HWND hWnd)
{
    PostMessage(DwmaxxRpcWindow(), DWMAXX_REMOVE_WINDOW, (WPARAM)hWnd, NULL);
}
