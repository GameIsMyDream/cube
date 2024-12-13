#pragma once

#include "timer.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <string>
#include <array>

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

YYZ_NAMESPACE_BEG

class Cube
{
public:
    Cube(HINSTANCE instance, int width, int height);
    ~Cube();
    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;

public:
    static Cube* Get();

public:
    void Init();
    int Run();

public:
    LRESULT CALLBACK WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

private:
    void InitWindow();
    bool InitDirect3d();
    void EnableDebugLayer();
    void CreateFactory();
    void CreateDevice();
    void CreateFence();
    void GetDescriptorSize();
    void CheckMsaa();
    void EnumAdaptors();
    void CreateCommandObjects();
    void CreateSwapChain();
    void CreateRtvDsvDescriptorHeaps();
    void CalculateFrameStats();
    void Update();
    void Draw();
    void FlushCommandQueue();
    void SetMsaaState(bool state);
    void OnResize();
    void OnLeftMouseUp(WPARAM btn_state, int x, int y);
    void OnLeftMouseDown(WPARAM btn_state, int x, int y);
    void OnMouseMove(WPARAM btn_state, int x, int y);
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

private:
    static Cube* cube;
    static constexpr UINT SWAP_CHAIN_BUFFER_COUNT = 2;

private:
    HINSTANCE       _instance   = NULL;
    HWND            _window     = NULL;
    int             _width      = 800;
    int             _height     = 600;
    bool            _paused     = false;
    bool            _minimized  = false;
    bool            _maximized  = false;
    bool            _resizing   = false;

    Timer           _timer;

    Microsoft::WRL::ComPtr<IDXGIFactory4>               _factory;
    Microsoft::WRL::ComPtr<IDXGISwapChain>              _swap_chain;
    Microsoft::WRL::ComPtr<ID3D12Device>                _device;
    Microsoft::WRL::ComPtr<ID3D12Fence>                 _fence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>          _command_queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      _command_allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>   _command_list;
    Microsoft::WRL::ComPtr<ID3D12Resource>              _swap_chain_buffer[SWAP_CHAIN_BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D12Resource>              _depth_stencil_buffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        _rtv_heap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        _dsv_heap;
    D3D12_VIEWPORT                                      _viewport;
    D3D12_RECT                                          _scissor_rect;

    int         _current_back_buffer            = 0;
    UINT64      _current_fence                  = 0;
    DXGI_FORMAT _back_buffer_format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT _depth_stencil_format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT        _rtv_descriptor_size            = 0;
    UINT        _dsv_descriptor_size            = 0;
    UINT        _cbv_srv_uav_descriptor_size    = 0;
    UINT        _msaa_quality                   = 0;
    bool        _msaa_state                     = false;
};

YYZ_NAMESPACE_END
