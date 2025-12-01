/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/SplashWindow.cpp
 *  PURPOSE:     This file implements the functionality to show a splash window
 *               before the game window appears. The splash window will be
 *               created and rendered on a separate thread to avoid blocking
                 the main thread.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "resource.h"
#include <chrono>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

static UINT GetWindowDpi(HWND window);
static SIZE GetPrimaryMonitorSize();
static LONG ScaleToDpi(LONG value, UINT dpi);

using hrc = std::chrono::high_resolution_clock;

///////////////////////////////////////////////////////////////////////////
//
// Splash window logic.
// 
///////////////////////////////////////////////////////////////////////////

class Splash final
{
public:
    bool CreateSplashWindow(HINSTANCE instance);
    void DestroySplashWindow();

    bool CreateDeviceResources();
    void ReleaseDeviceResources();

    bool Update();
    void Paint(HDC windowContext, bool drawBackground) const;
    void OnDestroy();

    void Show() const;
    void Hide() const;
    void BringToFront() const;

private:
    WNDCLASS m_windowClass{};
    HWND     m_window{};
    int      m_width{};
    int      m_height{};

    // Background bitmap
    HBITMAP m_bgBitmap{};
    HDC     m_bgContext{};


    Gdiplus::Bitmap* m_pSplashBitmap;  // ADD THIS
    ULONG_PTR m_gdiplusToken;           // ADD THIS
    RECT             m_previousRect;
};

static Splash g_splash{};

/**
 * @brief The window procedure for the splash window.
 */
static long CALLBACK HandleSplashWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    // NO LONGER SKIPPING WM_ERASEBKGND
    // if (message == WM_ERASEBKGND)
    //     return 1; 

    if (message == WM_PAINT)
    {
        PAINTSTRUCT ps{};
        HDC windowContext = BeginPaint(window, &ps);
        {
            // Now, we assume WM_ERASEBKGND happened or we draw on top.
            // The key is the logic inside Splash::Paint
            g_splash.Paint(windowContext, true); // Always draw the background (or clear)
        }
        EndPaint(window, &ps);
        return 0;
    }

    if (message == WM_DESTROY)
    {
        g_splash.OnDestroy();
        return 0;
    }

    return DefWindowProc(window, message, wParam, lParam);
}
/**
 * @brief Creates the splash window.
 * @param instance The instance handle of the loader DLL.
 * @return true if the window was created successfully, false otherwise.
 */
bool Splash::CreateSplashWindow(HINSTANCE instance)
{
    if (m_window != nullptr)
        return false;

    WNDCLASS windowClass{};
    windowClass.lpfnWndProc = &HandleSplashWindowMessage;
    windowClass.style = 0;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hIcon = LoadIconA(GetModuleHandle(nullptr), MAKEINTRESOURCE(110));            // IDI_ICON1 from Launcher
    windowClass.lpszClassName = TEXT("SplashWindow");

    if (!RegisterClass(&windowClass))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return false;
    }

    HWND window = CreateWindow(windowClass.lpszClassName, "Projet Monky Launcher", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL, NULL, instance, NULL);

    if (window == nullptr)
    {
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        return false;
    }

    // The source bitmap is 1000x500, but we render it at half size.
    SIZE monitorSize = GetPrimaryMonitorSize();
    UINT dpi = GetWindowDpi(window);
    m_width = ScaleToDpi(500, dpi);
    m_height = ScaleToDpi(250, dpi);
    int x = (monitorSize.cx - m_width) / 2;
    int y = (monitorSize.cy - m_height) / 2;
    MoveWindow(window, x, y, m_width, m_height, TRUE);
    ShowWindow(window, SW_SHOW);

    SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);

    m_windowClass = windowClass;
    m_window = window;
    return true;
}

/**
 * @brief Destroys the splash window and unregisters its window class.
 */
void Splash::DestroySplashWindow()
{
    ReleaseDeviceResources();

    if (m_window != nullptr)
    {
        DestroyWindow(m_window);
        m_window = {};
    }

    if (m_windowClass.hInstance != nullptr)
    {
        UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
        m_windowClass = {};
    }
}

/**
 * @brief Creates the device context and bitmap with the stretched splash image for rendering.
 */
bool Splash::CreateDeviceResources()
{
    if (!m_window)
        return false;

    // Initialize GDI+ once
    static bool gdiplusInitialized = false;
    if (!gdiplusInitialized)
    {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
        gdiplusInitialized = true;
    }

    // Load the banana image with transparency
    SString strPath = CalcMTASAPath("MTA\\cgui\\images\\banana.png");
    m_pSplashBitmap = Bitmap::FromFile(FromUTF8(strPath));
    
    if (m_pSplashBitmap == nullptr || m_pSplashBitmap->GetLastStatus() != Ok)
        return false;

    return true;
}

/**
 * @brief Releases the device context and bitmaps used for rendering.
 */
void Splash::ReleaseDeviceResources()
{
    if (m_bgContext)
    {
        DeleteDC(m_bgContext);
        m_bgContext = {};
    }

    if (m_bgBitmap)
    {
        DeleteObject(m_bgBitmap);
        m_bgBitmap = {};
    }
}

/**
 * @brief Polls the splash window message queue and updates
 */
/**
 * @brief Polls the splash window message queue and updates
 */
/**
 * @brief Polls the splash window message queue and updates
 */
bool Splash::Update()
{
    if (!m_window)
        return false;

    MSG msg{};

    while (PeekMessage(&msg, m_window, 0, 0, PM_NOREMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        if (GetMessage(&msg, m_window, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // **CRITICAL FIX: Invalidate the entire window area.**
    // FALSE tells Windows NOT to send a WM_ERASEBKGND message.
    // This allows WM_PAINT to fire every frame for animation.
    InvalidateRect(m_window, nullptr, FALSE); 
    
    return true;
}

/**
 * @brief Called when the splash window is destroyed.
 */
void Splash::OnDestroy()
{
    m_window = {};
}

/**
 * @brief Renders the splash window.
 * @param windowContext The device context of the window.
 * @param drawBackground If true, the background must be drawn.
 */
void Splash::Paint(HDC windowContext, bool drawBackground) const
{
    if (!drawBackground || !m_pSplashBitmap)
        return;

    // Create a compatible bitmap with 32-bit ARGB for transparency
    HDC memDC = CreateCompatibleDC(windowContext);
    HBITMAP memBitmap = CreateCompatibleBitmap(windowContext, m_width, m_height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    {
        // Use GDI+ to draw on our memory bitmap
        Graphics graphics(memDC);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetCompositingQuality(CompositingQualityHighQuality);
        graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        
        // Clear to fully transparent
        SolidBrush transparentBrush(Color(0, 0, 0, 0));
        graphics.FillRectangle(&transparentBrush, 0, 0, m_width, m_height);
        
        // Calculate rotation
        float angle = ((GetTickCount32() % 2000) / 2000.0f) * 360.0f;
        
        int imgWidth = m_pSplashBitmap->GetWidth();
        int imgHeight = m_pSplashBitmap->GetHeight();
        
        // Calculate center position within the window
        float centerX = m_width / 2.0f;
        float centerY = m_height / 2.0f;
        
        // Set up transformation for rotation around center of window
        graphics.TranslateTransform(centerX, centerY);  // Move to center of window
        graphics.RotateTransform(angle);                // Rotate
        graphics.TranslateTransform(-imgWidth / 2.0f, -imgHeight / 2.0f);  // Adjust for image center
        
        // Draw the rotating banana with its native transparency
        graphics.DrawImage(m_pSplashBitmap, 0, 0, imgWidth, imgHeight);
    }
    
    // Use UpdateLayeredWindow for true per-pixel transparency
    POINT ptDst = {0, 0};
    SIZE size = {m_width, m_height};
    POINT ptSrc = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    
    UpdateLayeredWindow(m_window, NULL, NULL, &size, memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    
    // Cleanup
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

/**
 * @brief Shows the splash window.
 */
void Splash::Show() const
{
    if (m_window)
    {
        ShowWindow(m_window, SW_SHOW);
    }
}

/**
 * @brief Hides the splash window.
 */
void Splash::Hide() const
{
    if (m_window)
    {
        ShowWindow(m_window, SW_HIDE);
    }
}

/**
 * @brief Brings the splash window to the front.
 */
void Splash::BringToFront() const
{
    if (m_window)
    {
        SetForegroundWindow(m_window);
        SetWindowPos(m_window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Splash thread logic.
//
///////////////////////////////////////////////////////////////////////////

class SplashThread final
{
public:
    void Create(HINSTANCE instance);
    void Destroy();

    bool Exists() const { return m_thread != nullptr; }
    bool ShouldExit() const { return m_exitEvent == nullptr || WaitForSingleObject(m_exitEvent, 0) == WAIT_OBJECT_0; }

    void Run(HINSTANCE instance);
    void PostRun();

private:
    HANDLE m_thread{};
    HANDLE m_exitEvent{};
};

static SplashThread g_splashThread{};

/**
 * @brief The entry point for the splash window thread.
 */
static DWORD WINAPI RunSplashThread(LPVOID instance)
{
    g_splashThread.Run(static_cast<HINSTANCE>(instance));
    g_splashThread.PostRun();
    return 0;
}

/**
 * @brief Creates the splash window thread.
 * @param instance The instance handle of the loader DLL.
 */
void SplashThread::Create(HINSTANCE instance)
{
    if (m_thread != nullptr)
        return;

    HANDLE exitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (exitEvent == nullptr)
        return;

    HANDLE thread = CreateThread(nullptr, 0, RunSplashThread, instance, 0, nullptr);

    if (thread == nullptr)
    {
        CloseHandle(exitEvent);
        return;
    }

    m_thread = thread;
    m_exitEvent = exitEvent;
}

/**
 * @brief Destroys the splash window thread and the splash window, if it still exists.
 */
void SplashThread::Destroy()
{
    if (m_thread)
    {
        SetEvent(m_exitEvent);
        WaitForSingleObject(m_thread, 3000);
        CloseHandle(m_thread);
        m_thread = {};
    }

    if (m_exitEvent)
    {
        CloseHandle(m_exitEvent);
        m_exitEvent = {};
    }

    g_splash.DestroySplashWindow();
}

/**
 * @brief The main loop of the splash window thread.
 * @param instance The instance handle of the loader DLL.
 */
void SplashThread::Run(HINSTANCE instance)
{
    if (!g_splash.CreateSplashWindow(instance))
        return;

    if (!g_splash.CreateDeviceResources())
    {
        g_splash.DestroySplashWindow();
        return;
    }

    MSG msg{};

    while (true)
    {
        if (g_splashThread.ShouldExit())
            break;

        if (!g_splash.Update())
            break;

        Sleep(10);
    }

    g_splash.DestroySplashWindow();
}

/**
 * @brief Cleans up the splash window thread on thread exit.
 */
void SplashThread::PostRun()
{
    if (m_exitEvent)
    {
        CloseHandle(m_exitEvent);
        m_exitEvent = {};
    }

    if (m_thread)
    {
        CloseHandle(m_thread);
        m_thread = {};
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Splash interface functions.
//
///////////////////////////////////////////////////////////////////////////

void ShowSplash(HINSTANCE instance)
{
    if (g_splashThread.Exists())
    {
        g_splash.BringToFront();
    }
    else
    {
        g_splashThread.Create(instance);
    }
}

void HideSplash()
{
    g_splashThread.Destroy();
    g_splash.DestroySplashWindow();
}

void SuspendSplash()
{
    g_splash.Hide();
}

void ResumeSplash()
{
    g_splash.Show();
}

///////////////////////////////////////////////////////////////////////////
//
// Utility functions.
//
///////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns the dots per inch (dpi) value for the specified window.
 */
static UINT GetWindowDpi(HWND window)
{
    // Minimum version: Windows 10, version 1607
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);

    static GetDpiForWindow_t Win32GetDpiForWindow = ([] {
        HMODULE user32 = LoadLibrary("user32");
        return user32 ? reinterpret_cast<GetDpiForWindow_t>(static_cast<void*>(GetProcAddress(user32, "GetDpiForWindow"))) : nullptr;
    })();

    if (Win32GetDpiForWindow)
        return Win32GetDpiForWindow(window);

    HDC  screenDC = GetDC(NULL);
    auto x = static_cast<UINT>(GetDeviceCaps(screenDC, LOGPIXELSX));
    ReleaseDC(NULL, screenDC);
    return x;
}

/**
 * @brief Returns the width and height of the primary monitor.
 */
static SIZE GetPrimaryMonitorSize()
{
    POINT    zero{};
    HMONITOR primaryMonitor = MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(primaryMonitor, &monitorInfo);

    const RECT& work = monitorInfo.rcWork;
    return {work.right - work.left, work.bottom - work.top};
}

/**
 * @brief Scales the value from the default screen dpi (96) to the provided dpi.
 */
static LONG ScaleToDpi(LONG value, UINT dpi)
{
    return static_cast<LONG>(ceil(value * static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI));
}
