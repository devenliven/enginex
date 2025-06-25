#include "pch.h"

#include "engine/core/window.h"
#include "engine/core/win32/os.h"
#include "utilities/logger.h"

#include "glad/glad.h"

Window::~Window()
{
    if (m_isOpen) {
        destroy();
    }
}

bool Window::create(const WindowData& data)
{
    if (m_isOpen) {
        LOG_WARN("Window is already open.");
        return false;
    }

    if (data.title.length() <= 0) {
        LOG_ERROR("Failed to create window, issue with window data.");
        return false;
    }

    m_title  = data.title;
    m_width  = data.width;
    m_height = data.height;

    // Register window class
    WNDCLASSEX wc        = {};
    HINSTANCE  hInstance = GetModuleHandle(nullptr);
    wc.cbSize            = sizeof(WNDCLASSEXW);
    wc.style             = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc       = WindowProc;
    wc.hInstance         = hInstance;
    wc.hIcon             = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor           = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName     = "EngineWindowClass";
    wc.hIconSm           = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        spdlog::error("Failed to register window class. Error: {}", GetLastError());
        return false;
    }

    RECT windowRect = {0, 0, m_width, m_height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    int windowWidth  = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    m_hwnd = CreateWindowEx(0, wc.lpszClassName, m_title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr, hInstance, this);

    if (!m_hwnd) {
        LOG_ERROR("Failed to create window. Error: {}", GetLastError());
        return false;
    }

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    m_hdc = GetDC(m_hwnd);
    if (!m_hdc) {
        LOG_ERROR("Failed to get device context.");
        destroy();
        return false;
    }

    if (!setupPixelFormat() || !createOpenGLContext()) {
        LOG_ERROR("Failed to setupPixelFormat!");
        destroy();
        return false;
    }

    if (!gladLoadGL()) {
        LOG_ERROR("Failed to initialize GLAD!");
        destroy();
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    m_isOpen = true;
    LOG_DEBUG("Window created succesfully: {}x{}", m_width, m_height);
    return true;
}

void Window::destroy()
{
    if (m_hglrc) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hglrc);
        m_hglrc = nullptr;
    }

    if (m_hdc) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
    }

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    m_isOpen = false;
    LOG_DEBUG("Window destroyed.");
}

void Window::pollEvents()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::swapBuffers()
{
    if (m_hdc) {
        SwapBuffers(m_hdc);
    }
}

bool Window::setupPixelFormat()
{
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize                 = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion              = 1;
    pfd.dwFlags               = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType            = PFD_TYPE_RGBA;
    pfd.cColorBits            = 32;
    pfd.cDepthBits            = 24;
    pfd.cStencilBits          = 8;
    pfd.iLayerType            = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    if (!pixelFormat) {
        LOG_ERROR("Failed to chose pixel format!");
        return false;
    }

    if (!SetPixelFormat(m_hdc, pixelFormat, &pfd)) {
        LOG_ERROR("Failed to set pixel format!");
        return false;
    }

    return true;
}

bool Window::createOpenGLContext()
{
    HGLRC tempContext = wglCreateContext(m_hdc);
    if (!tempContext) {
        LOG_ERROR("Failed to create temporary OpenGL context");
        return false;
    }

    if (!wglMakeCurrent(m_hdc, tempContext)) {
        LOG_ERROR("Failed to make temporary context current");
        wglDeleteContext(tempContext);
        return false;
    }

    // Try to create a modern context using WGL_ARB_create_context
    typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (wglCreateContextAttribsARB) {
        // Create OpenGL 4.6 Core context
        const int contextAttribs[] = {
            0x2091, 4,          // WGL_CONTEXT_MAJOR_VERSION_ARB
            0x2092, 6,          // WGL_CONTEXT_MINOR_VERSION_ARB
            0x9126, 0x00000001, // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
            0                   // End
        };

        m_hglrc = wglCreateContextAttribsARB(m_hdc, nullptr, contextAttribs);
    }

    // Clean up temporary context
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempContext);

    // Fall back to basic context if modern creation failed
    if (!m_hglrc) {
        LOG_WARN("Failed to create modern OpenGL context, falling back to basic context");
        m_hglrc = wglCreateContext(m_hdc);
    }

    if (!m_hglrc) {
        LOG_ERROR("Failed to create OpenGL context");
        return false;
    }

    if (!wglMakeCurrent(m_hdc, m_hglrc)) {
        LOG_ERROR("Failed to make OpenGL context current");
        return false;
    }
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (window) {
        return window->HandleMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CLOSE:
            m_isOpen = false;
            if (m_eventCallback) {
                // m_eventCallback(WindowEvent::Close, 0, 0, 0);
            }
            return 0;

        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                m_width  = LOWORD(lParam);
                m_height = HIWORD(lParam);
                if (m_eventCallback) {
                    // m_eventCallback(WindowEvent::Resize, m_width, m_height, 0);
                }
            }
            return 0;

        case WM_SETFOCUS:
            if (m_eventCallback) {
                // m_eventCallback(WindowEvent::Focus, 0, 0, 0);
            }
            return 0;

        case WM_KILLFOCUS:
            if (m_eventCallback) {
                // m_eventCallback(WindowEvent::LostFocus, 0, 0, 0);
            }
            return 0;

        case WM_MOVE:
            if (m_eventCallback) {
                // m_eventCallback(WindowEvent::Moved, LOWORD(lParam), HIWORD(lParam), 0);
            }
            return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (m_eventCallback) {
                // param1: virtual key code, param2: repeat count, param3: scan code
                int repeatCount = LOWORD(lParam);
                int scanCode    = (HIWORD(lParam) & 0xFF);
                // m_eventCallback(WindowEvent::KeyPressed, (int)wParam, repeatCount, scanCode);
            }
            return 0;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (m_eventCallback) {
                // param1: virtual key code, param2: 0, param3: scan code
                int scanCode = (HIWORD(lParam) & 0xFF);
                // m_eventCallback(WindowEvent::KeyReleased, (int)wParam, 0, scanCode);
            }
            return 0;

        case WM_CHAR:
            if (m_eventCallback) {
                // param1: character code, param2: repeat count
                int repeatCount = LOWORD(lParam);
                // m_eventCallback(WindowEvent::CharTyped, (int)wParam, repeatCount, 0);
            }
            return 0;

        // Mouse button events
        case WM_LBUTTONDOWN:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseButtonPressed, (int)MouseButton::Left, x, y);
            }
            return 0;

        case WM_LBUTTONUP:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseButtonReleased, (int)MouseButton::Left, x, y);
            }
            return 0;

        case WM_RBUTTONDOWN:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseButtonPressed, (int)MouseButton::Right, x, y);
            }
            return 0;

        case WM_RBUTTONUP:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseButtonReleased, (int)MouseButton::Right, x, y);
            }
            return 0;

        case WM_MBUTTONDOWN:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseButtonPressed, (int)MouseButton::Middle, x, y);
            }
            return 0;

        case WM_MBUTTONUP:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseButtonReleased, (int)MouseButton::Middle, x, y);
            }
            return 0;

        // Mouse movement
        case WM_MOUSEMOVE:
            if (m_eventCallback) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                // m_eventCallback(WindowEvent::MouseMoved, x, y, 0);
            }
            return 0;

        // Mouse wheel
        case WM_MOUSEWHEEL:
            if (m_eventCallback) {
                int x     = LOWORD(lParam);
                int y     = HIWORD(lParam);
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                // m_eventCallback(WindowEvent::MouseScrolled, x, y, delta);
            }
            return 0;

        case WM_MOUSEHWHEEL: // Horizontal scroll
            if (m_eventCallback) {
                int x     = LOWORD(lParam);
                int y     = HIWORD(lParam);
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                // m_eventCallback(WindowEvent::MouseScrolled, x, y, -delta); // Negative for horizontal
            }
            return 0;

        default: return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}
