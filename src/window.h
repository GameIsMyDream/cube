#pragma once

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <functional>

YYZ_NAMESPACE_BEG

class Window
{
public:
    Window(HINSTANCE instance, const std::wstring& caption);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

public:
    static Window* Get();
    
public:
    bool Init();
    int Run();

private:
    static LRESULT WINAPI WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

private:
    static Window* window;
    static constexpr TCHAR CLASS_NAME[] = L"Window";

private:
    HINSTANCE _instance;
    HWND _window;
    std::wstring _caption;
};

YYZ_NAMESPACE_END
