#include "cube.h"
#include "utils.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_cmd)
{
#if defined(DEBUG) || defined(_DEBUG)
    
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#endif
    
    try
    {
        yyz::Cube cube(instance, 960, 540);

        cube.Init();
        cube.Run();
    }
    catch (const yyz::D3dException& e)
    {
        ::MessageBox(NULL, e.What().c_str(), L"Error", 0);
    }
    catch (const std::exception& e)
    {
        ::MessageBox(NULL, yyz::Ansi2Unicode(e.what()).c_str(), L"Error", 0);
    }

    return 0;
}
