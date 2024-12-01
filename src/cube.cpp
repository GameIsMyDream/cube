#include "cube.h"
#include "utils.h"

#include <stdexcept>

YYZ_NAMESPACE_BEG

Cube::Cube(int width, int height) :
    _width(width),
    _height(height)
{
    ;
}

Cube::~Cube()
{
    ;
}

void Cube::Init()
{
    HRESULT res = 0;

    // 在调试模式下开启调试层

#if defined(DEBUG) || defined(_DEBUG)

    Microsoft::WRL::ComPtr<ID3D12Debug> debug;

    // THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));

    auto r = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
    if (!FAILED(r)) {
        throw yyz::D3dException(16, __FILE__, __LINE__);
        // throw std::runtime_error("11111");
    }

    debug->EnableDebugLayer();

#endif
}

void Cube::Draw()
{
    ;
}

int Cube::GetWidth() const
{
    return _width;
}

int Cube::GetHeight() const
{
    return _height;
}

YYZ_NAMESPACE_END

