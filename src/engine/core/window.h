#ifndef WINDOW_H_
#define WINDOW_H_

#include <windows.h>
#include <string>
#include <functional>

struct WindowData {
    std::string title;
    int         width;
    int         height;
};

// clang-format off

enum class WindowEvent { 
  Close, 
  Resize, 
  Focus, 
  LostFocus, 
  Moved,

  KeyPressed,
  KeyReleased,
  CharTyped,

  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled
};

enum class KeyCode
{
    Space = VK_SPACE,
    Enter = VK_RETURN,
    Escape = VK_ESCAPE,
    Tab = VK_TAB,
    Backspace = VK_BACK,
    Delete = VK_DELETE,
    
    // Arrow keys
    Left = VK_LEFT,
    Right = VK_RIGHT,
    Up = VK_UP,
    Down = VK_DOWN,
    
    // Function keys
    F1 = VK_F1, F2 = VK_F2, F3 = VK_F3, F4 = VK_F4,
    F5 = VK_F5, F6 = VK_F6, F7 = VK_F7, F8 = VK_F8,
    F9 = VK_F9, F10 = VK_F10, F11 = VK_F11, F12 = VK_F12,
    
    // Letters (A-Z)
    A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G',
    H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N',
    O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U',
    V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',
    
    // Numbers (0-9)
    Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
    Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',
    
    // Modifiers
    LeftShift = VK_LSHIFT,
    RightShift = VK_RSHIFT,
    LeftControl = VK_LCONTROL,
    RightControl = VK_RCONTROL,
    LeftAlt = VK_LMENU,
    RightAlt = VK_RMENU
};

enum class MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2
};
// clang-format on

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

  private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT                 HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool                    createOpenGLContext();
    bool                    setupPixelFormat();

    HWND  m_hwnd  = nullptr;
    HDC   m_hdc   = nullptr;
    HGLRC m_hglrc = nullptr;

    std::string m_title;
    int         m_width  = 0;
    int         m_height = 0;
    bool        m_isOpen = false;

    EventCallback m_eventCallback;
};

#endif // WINDOW_H_