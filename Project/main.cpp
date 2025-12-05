#include "Base/Framework.h"
using namespace KashipanEngine;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Framework framework;
    framework.Initialize("LE2A_09_スズキ_イオン", 1920, 1080, false);
    framework.Run();
    framework.Finalize();
    return 0;
}