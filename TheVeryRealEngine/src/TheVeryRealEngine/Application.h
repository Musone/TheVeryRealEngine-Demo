#pragma once

#include <string>

#include "Core.h"

namespace TVRE {

  class TVRE_API Application {
  protected:
    HWND app_window_;
    HANDLE process_;
    MSG message_;
    RECT window_rect_;
    float screen_center_x_;
    float screen_center_y_;

  public:
    int n_cmd_show;

    std::wstring process_name;
    std::wstring game_window_name;

    Application();
    virtual ~Application();

    void Init();

    virtual void Run() = 0;
  };

  // To be defined by the Client
  Application* CreateApplication();

}
