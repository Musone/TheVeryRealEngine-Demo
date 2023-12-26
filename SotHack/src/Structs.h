#pragma once

#include <TheVeryRealEngine.h>

class Guid {
public:
  int32_t A; //0x0000
  int32_t B; //0x0004
  int32_t C; //0x0008
  int32_t D; //0x000C

  bool operator==(const Guid& v) {
    return A == v.A && B == v.B && C == v.C && D == v.D;
  }

  bool operator!=(const Guid& v) {
    return A != v.A && B != v.B && C != v.C && D != v.D;
  }
};

struct WStringBuffer {
  wchar_t data[30];
};

struct StringBuffer {
  char data[60];
};

struct Camera {
  TVRE::Vector3 pos;
  TVRE::Vector3 angle;
  float fov;
};

struct Player {
  uint8_t ping;
  std::wstring name;
  uint32_t id;
  uint64_t ptr_rep_attach_component;
  uint64_t ptr_scene_component;
};

struct Crew {
  Guid crew_id;
  std::vector<Player> players;
  uint32_t num_players;
  bool has_ever_set_sail;
  uint64_t ptr_ship_rep_attach_component;
  std::wstring info_msg;
};

struct SceneComponentAndFName {
  uint64_t ptr_scene_component;
  std::wstring name;
};