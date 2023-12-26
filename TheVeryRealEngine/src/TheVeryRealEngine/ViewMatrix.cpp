#include <pch.h>

#include "ViewMatrix.h"

namespace TVRE {

  ViewMatrix::ViewMatrix(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23) {
    v_matrix_[0][0] = m00;	v_matrix_[0][1] = m01; v_matrix_[0][2] = m02; v_matrix_[0][3] = m03;
    v_matrix_[1][0] = m10;	v_matrix_[1][1] = m11; v_matrix_[1][2] = m12; v_matrix_[1][3] = m13;
    v_matrix_[2][0] = m20;	v_matrix_[2][1] = m21; v_matrix_[2][2] = m22; v_matrix_[2][3] = m23;
  }

  void ViewMatrix::SetViewMatrix(Vector3 rot) {
    const float rad_pitch = (rot.x * float(PI) / 180.f);
    const float rad_yaw = (rot.y * float(PI) / 180.f);
    const float rad_roll = (rot.z * float(PI) / 180.f);

    const float SP = sinf(rad_pitch);
    const float CP = cosf(rad_pitch);
    const float SY = sinf(rad_yaw);
    const float CY = cosf(rad_yaw);
    const float SR = sinf(rad_roll);
    const float CR = cosf(rad_roll);

    v_matrix_[0][0] = CP * CY;
    v_matrix_[0][1] = CP * SY;
    v_matrix_[0][2] = SP;
    v_matrix_[0][3] = 0.f;

    v_matrix_[1][0] = SR * SP * CY - CR * SY;
    v_matrix_[1][1] = SR * SP * SY + CR * CY;
    v_matrix_[1][2] = -SR * CP;
    v_matrix_[1][3] = 0.f;

    v_matrix_[2][0] = -(CR * SP * CY + SR * SY);
    v_matrix_[2][1] = CY * SR - CR * SP * SY;
    v_matrix_[2][2] = CR * CP;
    v_matrix_[2][3] = 0.f;

    v_axis_x_.x = v_matrix_[0][0], v_axis_x_.y = v_matrix_[0][1], v_axis_x_.z = v_matrix_[0][2];
    v_axis_y_.x = v_matrix_[1][0], v_axis_y_.y = v_matrix_[1][1], v_axis_y_.z = v_matrix_[1][2];
    v_axis_z_.x = v_matrix_[2][0], v_axis_z_.y = v_matrix_[2][1], v_axis_z_.z = v_matrix_[2][2];
  }

  void ViewMatrix::WorldToScreen(const float& screen_center_x, const float& screen_center_y, const float& fov,
                                 const Vector3& target_pos, const Vector3& player_pos, Vector2& out) {
    Vector3 vDelta = target_pos - player_pos;
    Vector3 vTransformed = Vector3(vDelta.Dot(v_axis_y_), vDelta.Dot(v_axis_z_), vDelta.Dot(v_axis_x_));

    if (vTransformed.z < 1.f) {
      vTransformed.z = 1.f;
    }

    auto tmp_fov = tanf(fov * (float)PI / 360.f);

    out.x = screen_center_x + vTransformed.x * (screen_center_x / tmp_fov) / vTransformed.z;
    out.y = screen_center_y - vTransformed.y * (screen_center_x / tanf(fov * PI / 360.f)) / vTransformed.z;
  }

}