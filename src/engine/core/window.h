#ifndef WINDOW_H_
#define WINDOW_H_

#include "engine/core/platform/windows/os.h"

#include <windows.h>
#include <string>
#include <functional>
#include <backends/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct WindowData {
    std::string title;
    int         width;
    int         height;
};

class Window
{
  public:
    using EventCallback = std::function<void(WindowEvent event, int param1, int param2, int param3)>;

    Window() = default;
    ~Window();

    bool create(const WindowData& data);
    void destroy();
    void pollEvents();
    void swapBuffers();

    void setEventCallback(EventCallback callback) { m_eventCallback = callback; }

    bool  isOpen() const { return m_isOpen; }
    int   getWidth() const { return m_width; }
    int   getHeight() const { return m_height; }
    HWND  getHandle() const { return m_hwnd; }
    HDC   getDeviceContext() const { return m_hdc; }
    HGLRC getOpenGLContext() const { return m_hglrc; }
    void  setUiMode(bool toggle);

    void showCursor(bool show);

  private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT                 HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool                    createOpenGLContext();
    bool                    setupPixelFormat();
    bool                    registerRawMouseInput();
    void                    confineCursor(bool confine);

    bool m_mouseInputRegistred = false;

    HWND  m_hwnd  = nullptr;
    HDC   m_hdc   = nullptr;
    HGLRC m_hglrc = nullptr;

    std::string m_title;
    int         m_width          = 0;
    int         m_height         = 0;
    bool        m_isOpen         = false;
    bool        m_cursorVisible  = true;
    bool        m_cursorConfined = false;
    bool        m_isUiActive     = false;

    EventCallback m_eventCallback;
};

#endif // WINDOW_H_