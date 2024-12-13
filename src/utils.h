#pragma once

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

#include <Windows.h>

#include <string>

YYZ_NAMESPACE_BEG

std::string Unicode2Utf8(const std::wstring& wstr);
std::wstring Ansi2Unicode(const std::string& str);
std::string GetLastErrorDescA(DWORD ec);
std::wstring GetLastErrorDescW(DWORD ec);

class D3dException
{
public:
    D3dException(HRESULT ec, const std::string& file, int line, const std::wstring& func);

public:
    std::wstring What() const;

private:
    HRESULT _ec;
    std::string _file;
    std::wstring _func;
    int _line;
};

#define THROW_ERROR(desc) \
    throw std::runtime_error(std::string(__FILE__) + ", line " + std::to_string(__LINE__) + "\n" + std::string(__FUNCTION__) + "\n" + std::string(desc));

#define THROW_D3D_ERROR(ec) \
    throw yyz::D3dException(ec, __FILE__, __LINE__, __FUNCTIONW__);

#define THROW_WIN32_ERROR(api, ec) \
    throw std::runtime_error(std::string(__FILE__) + ", line " + std::to_string(__LINE__) + "\n" + std::string(__FUNCTION__) + "\n" + \
    std::string(#api) + " failed with error(" + std::to_string(ec) + "): " + yyz::GetLastErrorDescA(ec));

YYZ_NAMESPACE_END
