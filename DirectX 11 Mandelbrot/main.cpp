#include "FractalEngine.h"
#include "WindowHelper.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstace, _In_ LPWSTR lpCmdLine, _In_ int nCmdShhow) 
{ 
    srand((unsigned)time(0));
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //Checks for memory leaks

    uint32_t WIDTH = 1024;
    uint32_t HEIGHT = 1024;

    HWND window;
    if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShhow, window)) {
        std::cerr << "Failed to setup window!" << std::endl;
        return -1;
    }

    FractalEngine mandelEngine(window, WIDTH, HEIGHT);

    mandelEngine.run();

    return 0;
}
