#include "pch.h"

#include "engine/core/window.h"
#include "engine/core/platform/windows/os.h"
#include "common/logger.h"
#include "common/stb_image.h"

#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <hidusage.h>

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
        LOG_ERROR("Failed to register window class. Error: {}", GetLastError());
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

    registerRawMouseInput();

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style                 = ImGui::GetStyle();
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_InitForOpenGL(m_hwnd);
    ImGui_ImplOpenGL3_Init();

    setUiMode(true);

    m_isOpen = true;
    LOG_DEBUG("Window created succesfully: {}x{}", m_width, m_height);
    return true;
}

void Window::destroy()
{
    LOG_INFO("Window: Attempting to destroy window.");

    confineCursor(false);
    showCursor(true);

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

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    UnregisterClass("EngineWindowClass", hInstance);

    m_isOpen = false;
    LOG_DEBUG("Window: Window destroyed!");
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
        const int contextAttribs[] = {
            0x2091, 4,          // WGL_CONTEXT_MAJOR_VERSION_ARB
            0x2092, 6,          // WGL_CONTEXT_MINOR_VERSION_ARB
            0x9126, 0x00000001, // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
            0                   // End
        };

        m_hglrc = wglCreateContextAttribsARB(m_hdc, nullptr, contextAttribs);
    }

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempContext);

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

    return true;
}

bool Window::registerRawMouseInput()
{
    RAWINPUTDEVICE rid[1];

    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags     = 0;
    rid[0].hwndTarget  = nullptr;

    if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE) {
        LOG_ERROR("Failed to register raw input device");
        return false;
    }

    m_mouseInputRegistred = true;
    LOG_INFO("Raw mouse input registered successfully");
    return true;
}

void Window::showCursor(bool show)
{
    m_cursorVisible = show;
    if (show) {
        while (ShowCursor(true) < 0)
            ;
    } else {
        while (ShowCursor(false) >= 0)
            ;
    }
}

void Window::confineCursor(bool confine)
{
    m_cursorConfined = confine;
    if (confine && m_hwnd) {
        RECT rect;
        GetClientRect(m_hwnd, &rect);

        POINT topLeft     = {rect.left, rect.top};
        POINT bottomRight = {rect.right, rect.bottom};

        ClientToScreen(m_hwnd, &topLeft);
        ClientToScreen(m_hwnd, &bottomRight);

        rect.left   = topLeft.x;
        rect.top    = topLeft.y;
        rect.right  = bottomRight.x;
        rect.bottom = bottomRight.y;

        ClipCursor(&rect);
    } else {
        ClipCursor(nullptr);
    }
}

void Window::setUiMode(bool toggle)
{
    m_isUiActive = toggle;

    if (GetFocus() == m_hwnd) {
        showCursor(m_isUiActive);
        confineCursor(!m_isUiActive);
    } else {
        showCursor(true);
        confineCursor(false);
    }

    ImGuiIO& io = ImGui::GetIO();
    if (m_isUiActive) {
        io.ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard);
    } else {
        io.ConfigFlags |= (ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard);
    }

    LOG_INFO("UI Mode Set to: {}", m_isUiActive);
}

void Window::restoreCursorState()
{
    if (GetFocus() == m_hwnd) {
        showCursor(m_isUiActive);
        confineCursor(!m_isUiActive);
    } else {
        showCursor(true);
        confineCursor(false);
    }
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (window && window->m_isUiActive) {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
            return true;
        }
    }

    if (window) {
        return window->HandleMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CLOSE:
            confineCursor(false);
            showCursor(true);
            m_isOpen = false;
            return 0;

        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                m_width  = LOWORD(lParam);
                m_height = HIWORD(lParam);

                // Only update cursor confinement if we're in editor and have focus
                if (!m_isUiActive && GetFocus() == m_hwnd) {
                    confineCursor(true);
                }
            }
            return 0;

        case WM_SETFOCUS:
            // Only manage cursor if we're in editor
            if (!m_isUiActive) {
                showCursor(false);
                confineCursor(true);
            }
            return 0;

        case WM_KILLFOCUS:
            showCursor(true);
            confineCursor(false);
            return 0;

        case WM_MOVE: return 0;

        case WM_EXITSIZEMOVE: {
            if (GetFocus() == m_hwnd && !m_isUiActive) {
                showCursor(false);
                confineCursor(true);
            }
            return 0;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam == VK_ESCAPE) {
                exit(0);
            }

            if (wParam == VK_TAB) {
                setUiMode(!m_isUiActive);
            }

            if (!m_isUiActive && m_eventCallback) {
                int repeatCount = LOWORD(lParam);
                int scanCode    = (HIWORD(lParam) & 0xFF);
                m_eventCallback(WindowEvent::KeyPressed, (int)wParam, repeatCount, scanCode);
            }
            return 0;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (!m_isUiActive && !ImGui::GetIO().WantCaptureKeyboard) {
                if (m_eventCallback) {
                    int scanCode = (HIWORD(lParam) & 0xFF);
                    m_eventCallback(WindowEvent::KeyReleased, (int)wParam, 0, scanCode);
                }
            }
            return 0;

        case WM_CHAR:
            if (m_eventCallback) {
                int repeatCount = LOWORD(lParam);
                // m_eventCallback(WindowEvent::CharTyped, (int)wParam, repeatCount, 0);
            }
            return 0;

        case WM_INPUT: {
            UINT dwSize = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

            if (dwSize > 0) {
                std::vector<BYTE> lpb(dwSize);
                if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
                    RAWINPUT* raw = (RAWINPUT*)lpb.data();

                    if (raw->header.dwType == RIM_TYPEMOUSE) {
                        if (!m_isUiActive && !ImGui::GetIO().WantCaptureMouse) {
                            if (m_eventCallback) {
                                int deltaX = raw->data.mouse.lLastX;
                                int deltaY = raw->data.mouse.lLastY;
                                m_eventCallback(WindowEvent::MouseMoved, deltaX, deltaY, 0);

                                if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
                                    short wheelDelta = (short)HIWORD(raw->data.mouse.usButtonData);
                                    m_eventCallback(WindowEvent::MouseScrolled, 0, 0, wheelDelta);
                                }
                            }
                        }
                    }
                }
            }
            return 0;
        }

        case WM_LBUTTONDOWN:
            if (!m_isUiActive && !ImGui::GetIO().WantCaptureMouse) {
                if (m_eventCallback) {
                    m_eventCallback(WindowEvent::MouseButtonPressed, (int)MouseButton::Left, 0, 0);
                }
            }
            return 0;

        case WM_LBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                if (m_eventCallback) {
                    m_eventCallback(WindowEvent::MouseButtonReleased, (int)MouseButton::Left, 0, 0);
                }
            }
            return 0;

        case WM_RBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                if (m_eventCallback) {
                    m_eventCallback(WindowEvent::MouseButtonPressed, (int)MouseButton::Right, 0, 0);
                }
            }
            return 0;

        case WM_RBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                if (m_eventCallback) {
                    m_eventCallback(WindowEvent::MouseButtonReleased, (int)MouseButton::Right, 0, 0);
                }
            }
            return 0;

        default: return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}