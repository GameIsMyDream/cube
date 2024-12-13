#include "utils.h"

#include <comdef.h>

#include <memory>

YYZ_NAMESPACE_BEG

std::string Unicode2Utf8(const std::wstring& wstr)
{
    auto size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::unique_ptr<char[]> buf(new char[size]);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buf.get(), size, NULL, NULL);
    return buf.get();
}

std::wstring Ansi2Unicode(const std::string& str)
{
    wchar_t buf[1024];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buf, 1024);
    return std::wstring(buf);
}

std::string GetLastErrorDescA(DWORD ec)
{
    constexpr DWORD bufsz = 1024;
    char buf[bufsz] = { 0 };

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ec, MAKELANGID(LANG_ARMENIAN, SUBLANG_DEFAULT), buf, bufsz, NULL);

    const auto len = strlen(buf);

    if (len > 1 && buf[len - 1] == '\n')
    {
        buf[len - 1] = 0;

        if (buf[len - 2] == '\r')
        {
            buf[len - 2] = 0;
        }
    }

    return buf;
}

std::wstring GetLastErrorDescW(DWORD ec)
{
    constexpr DWORD bufsz = 1024;
    wchar_t buf[bufsz] = { 0 };

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ec, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, bufsz, NULL);
    
    const auto len = wcslen(buf);

    if (len > 1 && buf[len - 1] == '\n')
    {
        buf[len - 1] = 0;

        if (buf[len - 2] == '\r')
        {
            buf[len - 2] = 0;
        }
    }

    return buf;
}

D3dException::D3dException(HRESULT ec, const std::string& file, int line, const std::wstring& func) :
    _ec(ec),
    _file(file),
    _func(func),
    _line(line)
{
    ;
}

std::wstring D3dException::What() const
{
    wchar_t buf[1024] = { 0 };
    swprintf_s(buf, L"%ls, line %d\n%ls\n(0x%X)%ls", Ansi2Unicode(_file).c_str(), _line, _func.c_str(), _ec, _com_error(_ec).ErrorMessage());
    return buf;
}

YYZ_NAMESPACE_END
