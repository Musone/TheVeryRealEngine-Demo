#include <pch.h>

#include "Graphics.h"

namespace TVRE {

  Graphics* Graphics::graphics_ = nullptr;

  Graphics::Graphics(HWND window_handle) {

    if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                          &factory_) != S_OK) {
      Utils::ThrowRuntimeError(L"Failed to create D2D1Factory");
    }

    if (DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                            __uuidof(IDWriteFactory),
                            (IUnknown**)&write_factory_) != S_OK) {
      Utils::ThrowRuntimeError(L"Failed to create DWriteFactory");
    }

    RECT rect;
    GetClientRect(window_handle, &rect);

    if (factory_->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
      ),
      D2D1::HwndRenderTargetProperties(
        window_handle,
        D2D1::SizeU(rect.right, rect.bottom),
        D2D1_PRESENT_OPTIONS_IMMEDIATELY // DISABLE VSYNC FOR FPS
      ),
      &render_target_
    ) != S_OK) {
      Utils::ThrowRuntimeError(L"Failed to create RenderTarget");
    };

    render_target_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    render_target_->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    #define FONT 20.f
    if (write_factory_->CreateTextFormat(
      L"Gabriola",
      NULL,
      DWRITE_FONT_WEIGHT_REGULAR,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      FONT,
      L"en-us",
      &text_format_
    ) != S_OK) {
      Utils::ThrowRuntimeError(L"Failed to create TextFormat");
    };

    if (render_target_->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f),
                                              &brush_) != S_OK) {
      Utils::ThrowRuntimeError(L"Failed to create SolidBrush");
    };

    vertex2_ = D2D1::Point2F((rect.right - rect.left) / 2.f - 10.f,
                             (rect.bottom - rect.top) / 2.f);
    vertex1_ = D2D1::Point2F(vertex2_.x - 10.f, vertex2_.y);
    vertex3_ = D2D1::Point2F(vertex2_.x + 10.f, vertex2_.y);
  }

  Graphics* Graphics::GetInstance() {
    if (graphics_ == nullptr) {
      Utils::ThrowRuntimeError(L"Please initialize Graphics before getting Graphics instance");
    }

    return graphics_;
  }

  Graphics::~Graphics() {
    if (factory_) factory_->Release();
    if (write_factory_) write_factory_->Release();
    if (text_format_) text_format_->Release();
    if (render_target_) render_target_->Release();
    if (brush_) brush_->Release();
  }

  bool Graphics::IsDrawing() { return isDrawing_; }

  void Graphics::BeginDraw() {
    render_target_->BeginDraw();
    isDrawing_ = true;
  }

  void Graphics::EndDraw() {
    render_target_->EndDraw();
    isDrawing_ = false;
  }

  void Graphics::ClearScreen() {
    render_target_->Clear(D2D1::ColorF(0, 0, 0, 0));
  }

  void Graphics::SetColor(const float& r, const float& g, const float& b, const float& a) {
    brush_->SetColor(D2D1::ColorF(r, g, b, a));
  }

  void Graphics::SetColor(const BYTE& r, const BYTE& g, const BYTE& b, const float& a) {
    brush_->SetColor(D2D1::ColorF(RGB(r, g, b), a));
  }
  void Graphics::DrawCircle(
    const float& x, const float& y,
    const float& radius, const float& thickness = 3.0f
  ) {
    render_target_->DrawEllipse(
      D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius),
      brush_, thickness
    );
  }

  void Graphics::DrawLine(const float& x1, const float& y1,
                          const float& x2, const float& y2,
                          const float& thickness) {
    vertex1_.x = x1, vertex1_.y = y1;
    vertex2_.x = x2, vertex2_.y = y2;
    render_target_->DrawLine(vertex1_, vertex2_, brush_, thickness, NULL);
  }

  void Graphics::DrawPoint(const float& x, const float& y, const float& thickness) {
    const float _y = y - thickness / 2.0f;
    DrawLine(x, _y, x, _y + thickness, thickness);
  }

  void Graphics::DrawText(std::wstring text, const int x, const int y,
                          const int width, const int height) {
    D2D1_RECT_F rect = { x, y, x + width, y + height };
    render_target_->DrawText(
      text.c_str(),
      text.size(),
      text_format_,
      rect,
      brush_,
      D2D1_DRAW_TEXT_OPTIONS_NONE,
      DWRITE_MEASURING_MODE_NATURAL
    );
  };

}