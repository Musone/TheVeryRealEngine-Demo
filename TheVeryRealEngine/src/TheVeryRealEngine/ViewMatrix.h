#pragma once

#include "TheVeryRealEngine/Vector3.h"
#include "TheVeryRealEngine/Vector2.h"

namespace TVRE {

  class TVRE_API ViewMatrix {
    float v_matrix_[3][4] = { 0 };
    Vector3 v_axis_x_;
    Vector3 v_axis_y_;
    Vector3 v_axis_z_;

  public:
    ViewMatrix() {};

    ViewMatrix(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13,
               float m20, float m21, float m22, float m23);

    float* operator[](int i) { Assert((i >= 0) && (i < 3)); return v_matrix_[i]; }

    /*
    const float *operator[](int i) const { Assert((i >= 0) && (i < 3)); return v_matrix_[i]; }
    float *Base() { return &v_matrix_[0][0]; }
    const float *Base() const { return &v_matrix_[0][0]; }
    */

    void SetViewMatrix(Vector3 rot);

    void WorldToScreen(const float& screen_center_x, const float& screen_center_y,
                       const float& fov, const Vector3& target_pos,
                       const Vector3& player_pos, Vector2& out);
  };
}