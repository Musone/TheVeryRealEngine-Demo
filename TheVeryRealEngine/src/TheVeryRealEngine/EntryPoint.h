#pragma once

#include <winbase.h>

#include "Core.h"

#ifdef TVRE_PLATFORM_WINDOWS

extern TVRE::Application* TVRE::CreateApplication();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT n_cmd_show) {
  auto app = TVRE::CreateApplication();
  app->n_cmd_show = n_cmd_show;
  app->Init();
  app->Run();
  delete app;

  return 0;
}

int main(int argc, char** argv) {
  HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(h_console, FOREGROUND_GREEN);

  return WinMain(NULL, NULL, NULL, 1);
}
#endif