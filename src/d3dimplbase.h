#pragma once

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

YYZ_NAMESPACE_BEG

class D3dImplBase
{
public:
    virtual void Init() = 0;
    virtual void Draw() = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
};

YYZ_NAMESPACE_END
