#include "window.h"
#include "utils.h"

#include <cassert>
#include <stdexcept>

YYZ_NAMESPACE_BEG

Window* Window::window = nullptr;

Window::Window(HINSTANCE instance, const std::string& caption, D3dImplBase& d3d_impl) :
    _instance(instance),
    _window(NULL),
    _caption(caption),
    _d3d_impl(d3d_impl)
{
    assert(!window);
    window = this;
}

Window* Window::Get()
{
    return window;
}

bool Window::Init()
{
    try
    {
        WNDCLASS wc;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProcedure;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = _instance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = CLASS_NAME;

        if (!::RegisterClass(&wc))
        {
            throw std::runtime_error("RegisterClass failed");
        }

        RECT rect = { 0, 0, _d3d_impl.GetWidth(), _d3d_impl.GetHeight() };
        ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;

        _window = ::CreateWindow(CLASS_NAME, _caption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, _instance, NULL);
        if (!_window)
        {
            throw std::runtime_error("CreateWindow failed");
        }

        auto sw = GetSystemMetrics(SM_CXFULLSCREEN);
        auto sh = GetSystemMetrics(SM_CYFULLSCREEN);
        ::MoveWindow(_window, (sw - w) / 2, (sh - h) / 2, w, h, false);

        ::ShowWindow(_window, SW_SHOW);
        ::UpdateWindow(_window);

        _d3d_impl.Init();
    }
    catch(const std::exception& e)
    {
        // ::MessageBox(NULL, Ansi2Unicode(e.what()).c_str(), L"Error", 0);
        ::MessageBoxA(NULL, e.what(), "Error", MB_OK);
        return false;
    }

    return true;
}

int Window::Run()
{
    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            Window::Get()->_d3d_impl.Draw();
        }
    }

    return (int)msg.wParam;
}

LRESULT Window::WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        return 0;

    case WM_SIZE:
        return 0;

    case WM_ENTERSIZEMOVE:
        return 0;

    case WM_EXITSIZEMOVE:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);

    case WM_GETMINMAXINFO:
        reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize.x = 200;
        reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize.y = 200;
        return 0;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        return 0;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        return 0;

    case WM_MOUSEMOVE:
        return 0;

    case WM_KEYUP:
        return 0;
    }

    return ::DefWindowProc(wnd, msg, wp, lp);
}

YYZ_NAMESPACE_END
