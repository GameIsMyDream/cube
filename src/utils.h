#pragma once

#define YYZ_NAMESPACE_BEG namespace yyz {
#define YYZ_NAMESPACE_END }

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <exception>
#include <string>

YYZ_NAMESPACE_BEG

std::wstring Ansi2Unicode(const std::string& str);
std::string Unicode2Ansi(const std::wstring& str);

class D3dException : public std::exception
{
public:
    D3dException(HRESULT res, const std::string& file, int line);

public:
    [[nodiscard]] const char* what() const override;

private:
    HRESULT _res;
    std::string _file;
    int _line;
};

#define THROW_IF_FAILED(r) if (FAILED(r)) { throw yyz::D3dException(r, __FILE__, __LINE__); }

YYZ_NAMESPACE_END
