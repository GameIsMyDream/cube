#include "utils.h"

#include <comdef.h>

YYZ_NAMESPACE_BEG

std::wstring Ansi2Unicode(const std::string& str)
{
    wchar_t buf[1024];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buf, 1024);
    return std::wstring(buf);
}

std::string Unicode2Ansi(const std::wstring& str)
{
    auto size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    char* buf = new char[size + 1];
    std::memset(buf, 0, size);
    WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, buf, size, NULL, NULL);
    std::string ret(buf);
    delete buf;
    return ret;
}

D3dException::D3dException(HRESULT res, const std::string& file, int line) :
    _res(res),
    _file(file),
    _line(line)
{
    ;
}

const char* D3dException::what() const
{
    return std::string(_file + ", Line " + std::to_string(_line) + ": " + std::to_string(_res)).c_str();
}

YYZ_NAMESPACE_END
