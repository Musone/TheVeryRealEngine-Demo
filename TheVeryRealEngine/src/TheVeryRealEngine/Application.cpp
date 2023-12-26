#include <pch.h>

#pragma comment(lib, "dwmapi")

#include <dwmapi.h>

#include "TheVeryRealEngine/ProcessIO.h"
#include "TheVeryRealEngine/Graphics.h"

#include "Application.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == WM_DESTROY) { PostQuitMessage(0); exit(0); return 0; }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

namespace TVRE {

  namespace WindowBuilder {

    static WNDCLASSEX CreateWindowClassHelper(const std::wstring& window_class_name) {
      WNDCLASSEX window_class{ sizeof(WNDCLASSEX) };
      window_class.hbrBackground = (HBRUSH)RGB(0, 0, 0);
      //window_class.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME;
      window_class.hInstance = GetModuleHandle(NULL);
      window_class.lpfnWndProc = WindowProc;
      window_class.lpszClassName = window_class_name.c_str();
      window_class.style = CS_HREDRAW | CS_VREDRAW;
      RegisterClassEx(&window_class);
      return window_class;
    };

    static HWND CreateWindowHelper(const WNDCLASSEX& window_class, const RECT& rect) {
      // create window. 
      //AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
      HWND app_window = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        window_class.lpszClassName,
        L"Extreemeeeee window",
        WS_POPUP,
        rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), nullptr
      );
      if (!app_window) {
        TVRE::Utils::ThrowRuntimeError(L"Failed to create window");
      }

      // LWA_COLORKEY is garbage and gimps the FPS
      if (!SetLayeredWindowAttributes(app_window, RGB(0, 0, 0), 255, LWA_ALPHA)) {
        TVRE::Utils::ThrowRuntimeError(L"Error updating window attributes");
      }

      MARGINS margin = { 0, 0, rect.right, rect.bottom };
      HRESULT dwmResult = DwmExtendFrameIntoClientArea(app_window, &margin);

      if (dwmResult != S_OK) {
        TVRE::Utils::ThrowRuntimeError(L"Error extending frame into client area");
      }

      return app_window;
    }

  }

  Application::Application() {
    ; // no-op
  }

  Application::~Application() {
    ; // no-op
  }

  void Application::Init() {
    process_ = TVRE::ProcessIO::GetProcessHandle(process_name);

    #pragma region Setup & create overlay
    // get game window.
    HWND game_window = FindWindow(NULL, game_window_name.c_str());
    if (!game_window) {
      TVRE::Utils::ThrowRuntimeError(L"Failed to create overlay window");
    }

    DWORD dw_style = GetWindowLongPtr(game_window, GWL_STYLE);
    DWORD dw_ex_style = GetWindowLongPtr(game_window, GWL_EXSTYLE);
    HMENU menu = GetMenu(game_window);

    // Get game's window dimensions
    GetWindowRect(game_window, &window_rect_);
    RECT rect2 = window_rect_;

    AdjustWindowRectEx(&rect2, dw_style, menu ? TRUE : FALSE, dw_ex_style);
    const DWORD dt = window_rect_.top - rect2.top;
    const DWORD dr = window_rect_.right - rect2.right;
    const DWORD dl = window_rect_.left - rect2.left;
    const DWORD db = window_rect_.bottom - rect2.bottom;
    window_rect_.top += dt;
    window_rect_.left += dl;
    window_rect_.right += dr;
    window_rect_.bottom += db;

    screen_center_x_ = (window_rect_.right - window_rect_.left) / 2.0f;
    screen_center_y_ = (window_rect_.bottom - window_rect_.top) / 2.0f;

    // Setup & create window
    const std::wstring window_class_name = L"window_class";
    WNDCLASSEX window_class = WindowBuilder::CreateWindowClassHelper(window_class_name);

    app_window_ = WindowBuilder::CreateWindowHelper(window_class, window_rect_);
    ShowWindow(app_window_, n_cmd_show);
    #pragma endregion

    TVRE::Logger::Init();
    TVRE_CORE_INFO("Initialized Logger!");

    TVRE::Graphics::Init(app_window_);
    TVRE_CORE_INFO("Initialized Graphics!");
  }

}
