#pragma once

#include <math.h>

#define PI 3.14159265358979323846f
#define RADPI 57.295779513082f
#define DEG2RAD(x)((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)((float)(x) * (float)(180.0f / (float)(PI)))

#define Assert( _exp ) ((void)0)

namespace TVRE {

  class TVRE_API Vector2 {
  public:
    Vector2(void) {
      x = y = 0.0f;
    }

    Vector2(float X, float Y) {
      x = X; y = Y;
    }

    Vector2(float* v) {
      x = v[0]; y = v[1];
    }

    Vector2(const float* v) {
      x = v[0]; y = v[1];
    }

    Vector2(const Vector2& v) {
      x = v.x; y = v.y;
    }

    Vector2& operator=(const Vector2& v) {
      x = v.x; y = v.y; return *this;
    }

    float& operator[](int i) {
      return ((float*)this)[i];
    }

    float operator[](int i) const {
      return ((float*)this)[i];
    }

    Vector2& operator+=(const Vector2& v) {
      x += v.x; y += v.y; return *this;
    }

    Vector2& operator-=(const Vector2& v) {
      x -= v.x; y -= v.y; return *this;
    }

    Vector2& operator*=(const Vector2& v) {
      x *= v.x; y *= v.y; return *this;
    }

    Vector2& operator/=(const Vector2& v) {
      x /= v.x; y /= v.y; return *this;
    }

    Vector2& operator+=(float v) {
      x += v; y += v; return *this;
    }

    Vector2& operator-=(float v) {
      x -= v; y -= v; return *this;
    }

    Vector2& operator*=(float v) {
      x *= v; y *= v; return *this;
    }

    Vector2& operator/=(float v) {
      x /= v; y /= v; return *this;
    }

    Vector2 operator+(const Vector2& v) const {
      return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const Vector2& v) const {
      return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(const Vector2& v) const {
      return Vector2(x * v.x, y * v.y);
    }

    Vector2 operator/(const Vector2& v) const {
      return Vector2(x / v.x, y / v.y);
    }

    Vector2 operator+(float v) const {
      return Vector2(x + v, y + v);
    }

    Vector2 operator-(float v) const {
      return Vector2(x - v, y - v);
    }

    Vector2 operator*(float v) const {
      return Vector2(x * v, y * v);
    }

    Vector2 operator/(float v) const {
      return Vector2(x / v, y / v);
    }

    void Set(float X = 0.0f, float Y = 0.0f) {
      x = X; y = Y;
    }

    float Length(void) const {
      return sqrtf(x * x + y * y);
    }

    float LengthSqr(void) const {
      return (x * x + y * y);
    }

    float DistTo(const Vector2& v) const {
      return (*this - v).Length();
    }

    float DistToSqr(const Vector2& v) const {
      return (*this - v).LengthSqr();
    }

    float Dot(const Vector2& v) const {
      return (x * v.x + y * v.y);
    }

    bool IsZero(void) const {
      return (x > -0.01f && x < 0.01f &&
              y > -0.01f && y < 0.01f);
    }

    inline void Rotate2D(const float& f) {
      float _x, _y;
      float s, c;

      s = sin(DEG2RAD(f));
      c = cos(DEG2RAD(f));

      _x = x;
      _y = y;

      x = (_x * c) - (_y * s);
      y = (_x * s) + (_y * c);
    }

  public:
    float x, y;
  };

}