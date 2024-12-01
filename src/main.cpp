#include "d3dimplbase.h"
#include "window.h"
#include "cube.h"

#include <Windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    yyz::Cube cube(960, 540);
    yyz::Window w(instance, "Cube", cube);
    w.Init();
    w.Run();

    return 0;
}
