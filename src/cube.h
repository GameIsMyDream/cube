#pragma once

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

#include "d3dimplbase.h"
#include "timer.h"

#include <wrl.h>
#include <d3d12.h>

YYZ_NAMESPACE_BEG

class Cube : public D3dImplBase
{
public:
    Cube(int width, int height);
    ~Cube();

public:
    // void Init();

public:
    void Init() override;
    void Draw() override;
    int GetWidth() const override;
    int GetHeight() const override;

private:
    int _width;
    int _height;
    Timer _timer;
};

YYZ_NAMESPACE_END
