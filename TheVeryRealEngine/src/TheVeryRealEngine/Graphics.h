#pragma once

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#include <d2d1.h>
#include <dwrite.h>
#include <string>

namespace TVRE {

  class TVRE_API Graphics {
    ID2D1Factory* factory_ = nullptr;

    IDWriteFactory* write_factory_ = nullptr;
    IDWriteTextFormat* text_format_ = nullptr;

    ID2D1HwndRenderTarget* render_target_ = nullptr;
    ID2D1SolidColorBrush* brush_ = nullptr;

    D2D1_POINT_2F vertex1_;
    D2D1_POINT_2F vertex2_;
    D2D1_POINT_2F vertex3_;

    static Graphics* graphics_;

    bool isDrawing_ = false;

    Graphics(HWND window_handle);

  public:
    static void Init(HWND window_handle) {
      if (graphics_) {
        delete graphics_;
      }

      graphics_ = new Graphics(window_handle);
    }

    static Graphics* GetInstance();

    ~Graphics();

    bool IsDrawing();

    void BeginDraw();

    void EndDraw();

    void ClearScreen();

    void SetColor(const float& r, const float& g, const float& b, const float& a);

    void SetColor(const BYTE& r, const BYTE& g, const BYTE& b, const float& a);

    void DrawCircle(
      const float& x, const float& y,
      const float& radius, const float& thickness
    );

    void DrawLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& thickness);

    void DrawPoint(const float& x, const float& y, const float& thickness);

    void DrawText(std::wstring text, const int x = 100, const int y = 100, const int width = 500, const int height = 0);
  };

}