#include "d3dx12.h"
#include "cube.h"
#include "utils.h"

#include <windowsx.h>
#include <DirectXColors.h>

#include <cassert>
#include <vector>
#include <stdexcept>

YYZ_NAMESPACE_BEG

LRESULT CALLBACK CubeWindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    return Cube::Get()->WindowProcedure(wnd, msg, wp, lp);
}

Cube* Cube::cube = nullptr;

Cube::Cube(HINSTANCE instance, int width, int height) :
    _instance(instance),
    _width(width),
    _height(height)
{
    assert(!cube);
    cube = this;
}

Cube::~Cube()
{
    if (_device)
    {
        FlushCommandQueue();
    }
}

Cube* Cube::Get()
{
    return cube;
}

void Cube::Init()
{
    InitWindow();
    InitDirect3d();
    OnResize();
}

int Cube::Run()
{
    MSG msg = { 0 };

    _timer.Reset();

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            _timer.Tick();

            if (_paused)
            {
                Sleep(100);
            }
            else
            {
                CalculateFrameStats();
                Update();
                Draw();
            }
        }
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK Cube::WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_ACTIVATE: // 窗口激活状态改变
    {
        if (LOWORD(wp) == WA_INACTIVE)
        {
            _paused = true;
            _timer.Stop();
        }
        else
        {
            _paused = false;
            _timer.Start();
        }

        return 0;
    }

    case WM_SIZE: // 调整窗口大小
    {
        _width = LOWORD(lp);
        _height = HIWORD(lp);

        if (_device)
        {
            if (wp = SIZE_MINIMIZED)
            {
                _paused = true;
                _minimized = true;
                _maximized = false;
            }
            else if (wp = SIZE_MAXIMIZED)
            {
                _paused = false;
                _minimized = false;
                _maximized = true;

                OnResize();
            }
            else if (wp = SIZE_RESTORED)
            {
                if (_minimized)
                {
                    _paused = false;
                    _minimized = false;

                    OnResize();
                }
                else if (_maximized)
                {
                    _paused = false;
                    _maximized = false;

                    OnResize();
                }
                else if (_resizing)
                {
                    ; // 调整窗口大小过程中不做处理
                }
                else // 例如调用 SetWindowPos 或 IDXGISwapChain::SetFullscreenState
                {
                    OnResize();
                }
            }
        }

        return 0;
    }

    case WM_ENTERSIZEMOVE: // 调整窗口大小开始
    {
        _paused = true;
        _resizing = true;

        _timer.Stop();

        return 0;
    }

    case WM_EXITSIZEMOVE: // 调整窗口大小结束
    {
        _paused = false;
        _resizing = false;

        _timer.Start();
        OnResize();

        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);

    case WM_GETMINMAXINFO: // 当窗口的大小或位置即将更改时
    {
        // 设置窗口的最小尺寸

        auto min_max_info = reinterpret_cast<MINMAXINFO*>(lp);
        if (min_max_info)
        {
            min_max_info->ptMinTrackSize.x = 200;
            min_max_info->ptMinTrackSize.y = 200;
        }

        return 0;
    }

    case WM_LBUTTONDOWN: // 按下鼠标左键
        return 0;
        
    case WM_RBUTTONDOWN: // 按下鼠标右键
    {
        OnLeftMouseDown(wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        return 0;
    }

    case WM_MBUTTONDOWN: // 按下鼠标中键
        return 0;

    case WM_LBUTTONUP: // 释放鼠标左键
    {
        OnLeftMouseUp(wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        return 0;
    }

    case WM_RBUTTONUP: // 释放鼠标右键
        return 0;

    case WM_MBUTTONUP: // 释放鼠标中键
        return 0;

    case WM_MOUSEMOVE: // 移动鼠标
    {
        OnMouseMove(wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        return 0;
    }

    case WM_KEYUP: // 释放一个非系统键
    {
        if (wp == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        else if (static_cast<int>(wp) == VK_F2)
        {
            SetMsaaState(!_msaa_state);
        }

        return 0;
    }

    default:
        break;
    }

    return ::DefWindowProc(wnd, msg, wp, lp);
}

void Cube::InitWindow()
{
    const wchar_t CLASS_NAME[] = L"CubeWindow";

    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = CubeWindowProcedure;
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
        THROW_WIN32_ERROR(RegisterClass, GetLastError());
    }

    RECT rect = { 0, 0, _width, _height };
    ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int x = (GetSystemMetrics(SM_CXFULLSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYFULLSCREEN) - h) / 2;

    _window = ::CreateWindow(CLASS_NAME, L"Cube", WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, _instance, NULL);
    if (!_window)
    {
        THROW_WIN32_ERROR(CreateWindow, GetLastError());
    }

    ::ShowWindow(_window, SW_SHOW);
    ::UpdateWindow(_window);
}

bool Cube::InitDirect3d()
{
    EnableDebugLayer();
    CreateFactory();
    CreateDevice();
    CreateFence();
    GetDescriptorSize();
    CheckMsaa();
    // EnumAdaptors();
    CreateCommandObjects();
    CreateSwapChain();
    CreateRtvDsvDescriptorHeaps();

    return true;
}

void Cube::EnableDebugLayer()
{
#if defined(DEBUG) || defined(_DEBUG) // 仅在调试模式下开启调试层

    Microsoft::WRL::ComPtr<ID3D12Debug> debug;

    auto ec = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    debug->EnableDebugLayer();

#endif
}

void Cube::CreateFactory()
{
    auto ec = CreateDXGIFactory1(IID_PPV_ARGS(_factory.GetAddressOf())); // 创建可用于生成其他 DXGI 对象的工厂
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }
}

void Cube::CreateDevice()
{
    auto ec = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(_device.GetAddressOf())); // 使用默认显示适配器创建设备

    if (FAILED(ec))
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> warp_adaptor;

        ec = _factory->EnumWarpAdapter(IID_PPV_ARGS(warp_adaptor.GetAddressOf())); // 枚举WARP适配器
        if (FAILED(ec))
        {
            THROW_D3D_ERROR(ec);
        }

        ec = D3D12CreateDevice(warp_adaptor.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(_device.GetAddressOf())); // 回退到WARP设备
        if (FAILED(ec))
        {
            THROW_D3D_ERROR(ec);
        }
    }
}

void Cube::CreateFence()
{
    auto ec = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.GetAddressOf())); // 创建围栏对象
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }
}

void Cube::GetDescriptorSize()
{
    // 获取描述符堆的句柄增量大小

    _rtv_descriptor_size = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _dsv_descriptor_size = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    _cbv_srv_uav_descriptor_size = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Cube::CheckMsaa()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS mql;
    mql.Format = _back_buffer_format;
    mql.SampleCount = 4;
    mql.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    mql.NumQualityLevels = 0;

    auto ec = _device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &mql, sizeof(mql)); // 获取图形驱动程序支持的多重采样的质量级别
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    _msaa_quality = mql.NumQualityLevels;
    if (_msaa_quality <= 0)
    {
        THROW_ERROR("Unexpected MSAA quality level");
    }
}

void Cube::EnumAdaptors()
{
#if defined(DEBUG) || defined(_DEBUG)
    
    UINT i = 0;
    IDXGIAdapter1* adaptor = nullptr;
    std::vector<IDXGIAdapter1*> adaptors;
    std::wstring str;

    while (_factory->EnumAdapters1(i++, &adaptor) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 adaptor_desc = {};
        adaptor->GetDesc1(&adaptor_desc);
        adaptors.push_back(adaptor);
        str.append(L"Adaptor: ");
        str.append(adaptor_desc.Description);
        str.append(L"\n");

        UINT j = 0;
        IDXGIOutput* output = nullptr;

        while (adaptor->EnumOutputs(j++, &output) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_OUTPUT_DESC output_desc = {};
            output->GetDesc(&output_desc);
            str.append(L"    Output: ");
            str.append(output_desc.DeviceName);
            str.append(L"\n");

            // UINT mode_count = 0;
            // std::vector<DXGI_MODE_DESC> mode_descs = {};
            // output->GetDisplayModeList(_back_buffer_format, 0, &mode_count, nullptr);
            // mode_descs.resize(mode_count);
            // output->GetDisplayModeList(_back_buffer_format, 0, &mode_count, mode_descs.data());

            // for (auto desc : mode_descs)
            // {
            //     str.append(L"        Width: " + std::to_wstring(desc.Width));
            //     str.append(L"  Height: " + std::to_wstring(desc.Height));
            //     str.append(L"  Refresh: " + std::to_wstring(desc.RefreshRate.Numerator) + L" / " + std::to_wstring(desc.RefreshRate.Denominator));
            //     str.append(L"\n");
            // }
        }
    }

    ::MessageBox(NULL, str.c_str(), L"Error", 0);

#endif
}

void Cube::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    auto ec = _device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(_command_queue.GetAddressOf())); // 创建命令队列
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    ec = _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_command_allocator.GetAddressOf())); // 创建命令分配器对象
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    ec = _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _command_allocator.Get(), nullptr, IID_PPV_ARGS(_command_list.GetAddressOf())); // 创建命令列表
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    _command_list->Close();
}

void Cube::CreateSwapChain()
{
    _swap_chain.Reset();

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = _width;
    desc.BufferDesc.Height = _height;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = _back_buffer_format;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = _msaa_state ? 4 : 1;
    desc.SampleDesc.Quality = _msaa_state ? (_msaa_quality - 1) : 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    desc.OutputWindow = _window;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Windowed = true;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    auto ec = _factory->CreateSwapChain(_command_queue.Get(), &desc, _swap_chain.GetAddressOf()); // 创建交换链
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }
}

void Cube::CreateRtvDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtv_desc = {};
    rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_desc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtv_desc.NodeMask = 0;

    auto ec = _device->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(_rtv_heap.GetAddressOf())); // 创建渲染目标视图的描述符堆
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsv_desc = {};
    dsv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_desc.NumDescriptors = 1;
    dsv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsv_desc.NodeMask = 0;

    ec = _device->CreateDescriptorHeap(&dsv_desc, IID_PPV_ARGS(_dsv_heap.GetAddressOf())); // 创建深度模板视图的描述符堆
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }
}

void Cube::CalculateFrameStats()
{
    static float elapsed = 0.0f;
    static int frame_count = 0;

    ++frame_count;

    if ((_timer.TotalTime() - elapsed) >= 1.0f)
    {
        std::wstring text = L"FPS: " + std::to_wstring(frame_count) + L"    MSPF: " + std::to_wstring(1000.0f / frame_count);
        ::SetWindowText(_window, text.c_str());

        elapsed += 1.0f;
        frame_count = 0;
    }
}

void Cube::Update()
{
    ;
}

void Cube::Draw()
{
    auto ec = _command_allocator->Reset();
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    ec = _command_list->Reset(_command_allocator.Get(), nullptr);
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    _command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_swap_chain_buffer[_current_back_buffer].Get(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    _command_list->RSSetViewports(1, &_viewport);
    _command_list->RSSetScissorRects(1, &_scissor_rect);

    _command_list->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::Cyan, 0, nullptr);
    _command_list->ClearDepthStencilView(_dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    _command_list->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &_dsv_heap->GetCPUDescriptorHandleForHeapStart());

    _command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_swap_chain_buffer[_current_back_buffer].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ec = _command_list->Close();
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    std::array<ID3D12CommandList*, 1> cmd_lists = { _command_list.Get() };
    _command_queue->ExecuteCommandLists(static_cast<UINT>(cmd_lists.size()), cmd_lists.data());

    ec = _swap_chain->Present(0, 0);
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    _current_back_buffer = (_current_back_buffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

    FlushCommandQueue();
}

void Cube::FlushCommandQueue()
{
    ++_current_fence;

    auto ec = _command_queue->Signal(_fence.Get(), _current_fence);
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    if (_fence->GetCompletedValue() < _current_fence)
    {
        auto event = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

        ec = _fence->SetEventOnCompletion(_current_fence, event);
        if (FAILED(ec))
        {
            THROW_D3D_ERROR(ec);
        }

        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

void Cube::SetMsaaState(bool state)
{
    if (_msaa_state != state)
    {
        _msaa_state = state;

        // 重建交换链和缓冲区

        CreateSwapChain();
        OnResize();
    }
}

void Cube::OnResize()
{
    if (!_device)
    {
        THROW_ERROR("D3D device is null");
    }

    if (!_swap_chain)
    {
        THROW_ERROR("D3D swap chain is null");
    }

    if (!_command_allocator)
    {
        THROW_ERROR("D3D command allocator is null");
    }

    FlushCommandQueue();

    // 重置命令列表

    auto ec = _command_list->Reset(_command_allocator.Get(), nullptr);
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        _swap_chain_buffer[i].Reset();
    }
    _depth_stencil_buffer.Reset();

    ec = _swap_chain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, _width, _height, _back_buffer_format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    _current_back_buffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap(_rtv_heap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        ec = _swap_chain->GetBuffer(i, IID_PPV_ARGS(_swap_chain_buffer[i].GetAddressOf()));
        if (FAILED(ec))
        {
            THROW_D3D_ERROR(ec);
        }

        _device->CreateRenderTargetView(_swap_chain_buffer[i].Get(), nullptr, rtv_heap);
        rtv_heap.Offset(1, _rtv_descriptor_size);
    }

    D3D12_RESOURCE_DESC ds_desc = {};
    ds_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    ds_desc.Alignment = 0;
    ds_desc.Width = _width;
    ds_desc.Height = _height;
    ds_desc.DepthOrArraySize = 1;
    ds_desc.MipLevels = 1;
    ds_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    ds_desc.SampleDesc.Count = _msaa_state ? 4 : 1;
    ds_desc.SampleDesc.Quality = _msaa_state ? (_msaa_quality - 1) : 0;
    ds_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    ds_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = _depth_stencil_format;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;

    ec = _device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
        &ds_desc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(_depth_stencil_buffer.GetAddressOf()));
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = _depth_stencil_format;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
    dsv_desc.Texture2D.MipSlice = 0;

    _device->CreateDepthStencilView(_depth_stencil_buffer.Get(), &dsv_desc, _dsv_heap->GetCPUDescriptorHandleForHeapStart());

    _command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

    ec = _command_list->Close();
    if (FAILED(ec))
    {
        THROW_D3D_ERROR(ec);
    }

    std::array<ID3D12CommandList*, 1> cmd_lists = { _command_list.Get() };
    _command_queue->ExecuteCommandLists(static_cast<UINT>(cmd_lists.size()), cmd_lists.data());

    FlushCommandQueue();

    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;
    _viewport.Width = static_cast<FLOAT>(_width);
    _viewport.Height = static_cast<FLOAT>(_height);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;

    _scissor_rect = { 0, 0, _width, _height };
}

void Cube::OnLeftMouseUp(WPARAM btn_state, int x, int y)
{
    ;
}

void Cube::OnLeftMouseDown(WPARAM btn_state, int x, int y)
{
    ;
}

void Cube::OnMouseMove(WPARAM btn_state, int x, int y)
{
    ;
}

D3D12_CPU_DESCRIPTOR_HANDLE Cube::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtv_heap->GetCPUDescriptorHandleForHeapStart(), _current_back_buffer, _rtv_descriptor_size);
}

YYZ_NAMESPACE_END

