#include "render.h"
#include <thread>

Memory m;
render D;

// Overlay
HWND hwnd;
HWND GHwnd;
bool ShowMenu = true;
void OverlayManager();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Init
    if (!m.Init())
        return 0;

    // Get R5R Window info
    GHwnd = FindWindowA(NULL, "Apex Legends");
    GetClientRect(GHwnd, &g.GameSize);
    ClientToScreen(GHwnd, &g.GamePoint);

    // Create Overlay
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, "FUCK YOU R5Reloaded", "R5R", NULL };
    RegisterClassExA(&wc);
    hwnd = CreateWindowExA(NULL, wc.lpszClassName, wc.lpszMenuName, WS_POPUP | WS_VISIBLE, g.GamePoint.x, g.GamePoint.y, g.GameSize.right, g.GameSize.bottom, NULL, NULL, wc.hInstance, NULL);
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        exit(0);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;

    // You can load fonts and ImGui style
    LoadStyle();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Background color
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);

    // Main loop
    g.Active = true;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OverlayManager, NULL, 0, NULL);

    while (g.Active)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                g.Active = false;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        HWND R5R = FindWindowA(NULL, "Apex Legends");
        if (R5R)
        {
            HWND TempProcessHwnd = GetWindow(R5R, GW_HWNDPREV);
            SetWindowPos(hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        // Info
        D.m_Info();

        // Menu
        if (ShowMenu)
            D.m_Menu();

        // ESP
        if (g.ESP)
            D.m_ESP();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (g.VSync)
            g_pSwapChain->Present(1, 0);	// vsync off
        else
            g_pSwapChain->Present(0, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

    return 0;
}

void OverlayManager()
{
    while (g.Active)
    {
        // ShowMenu
        static bool tmp = false;
        static bool change = false;
        if (IsKeyDown(VK_F11) && !tmp)
        {
            ShowMenu = !ShowMenu;
            tmp = true;
            change = false;
        }
        else if (!IsKeyDown(VK_F11) && tmp)
        {
            tmp = false;
        }

        // Window style
        if (ShowMenu && !change)
        {
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED);
            change = false;

        }
        else if (!change)
        {
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
            change = false;
        }

        // Window Mover
        HWND TempHwnd = FindWindowA(NULL, "Apex Legends");
        if (TempHwnd)
        {
            RECT tmpRect = {};
            POINT tmpPos = {};
            GetClientRect(TempHwnd, &tmpRect);
            ClientToScreen(TempHwnd, &tmpPos);

            if (g.GamePoint.x != tmpPos.x || g.GamePoint.x != tmpPos.y)
                SetWindowPos(hwnd, NULL, tmpPos.x, tmpPos.y, tmpRect.right, tmpRect.bottom, SWP_NOREDRAW);

            g.GameSize = tmpRect;
            g.GamePoint = tmpPos;
        }
        else if (!TempHwnd)
        {
            g.Active = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ExitThread(0);
}