#pragma once

#include <TheVeryRealEngine.h>

#include "..\Structs.h"

typedef struct SotPosAndAngle {
  float x;
  float y;
  float z;
  float pitch;
  float yaw;
  float roll;
} SotPosAndAngle;

struct AActor {
  char pad_0000[24]; //0x0000
  uint32_t fNameIndex;
  char pad_0001[228]; //0x0000
  uint64_t RepAttachment; //0x0100
  char pad_0108[96]; //0x0108
  uint64_t USceneComponent; //0x0168
  char pad_0170[604]; //0x0170
  uint32_t Ping; //0x03CC
  uint64_t PlayerName; //0x03D0
  uint32_t PlayerNameLength; //0x03D8
  char pad_03DC[20]; //0x03DC
  int32_t PlayerId; //0x03F0
};
static_assert(sizeof(AActor) == 0x3F8);

struct USceneComponent {
  char pad_0000[264]; //0x0000
  TVRE::Vector3 RelativeScale3D; //0x0108
  TVRE::Vector3 RelativeRotation; //0x0114
  TVRE::Vector3 idk; //0x0120
  TVRE::Vector3 absCoord1; //0x012C
}; //Size: 0x0138
static_assert(sizeof(USceneComponent) == 0x138);


struct TArray {
  uint64_t ptr_base;
  uint32_t size;
}; //Size: 0x10
static_assert(sizeof(TArray) == 0x10);

struct AGameState {
  char pad_0000[1008]; //0x0000
  uint32_t ElapsedTime; //0x03F0
  char pad_03F4[4]; //0x03F4
  TArray player_list;
  float ReplicatedWorldTimeSeconds; //0x0408
  float ServerWorldTimeSecondsDelta; //0x040C
  float ServerWorldTimeSecondsUpdateFrequency; //0x0410
  char pad_0414[436]; //0x0414
  uint64_t ShipService; //0x05C0
  char pad_05C8[56]; //0x05C8
  uint64_t CrewService; //0x0600
}; //Size: 0x0608
static_assert(sizeof(AGameState) == 0x610);

struct SotCamera {
  TVRE::Vector3 pos;
  TVRE::Vector3 angle;
  char pad[16];
  float fov;
};

class CrewShipEntry : public Guid {
public:
  uint64_t ptr_ship; //0x0010
}; //Size: 0x0018

class FTransform
{
public:
  float rotation_quat_x; //0x0000
  float rotation_quat_y; //0x0000
  float rotation_quat_z; //0x0000
  float rotation_quat_w; //0x0000
  TVRE::Vector3 translation; //0x0010
  char pad_001C[4]; //0x001C
  TVRE::Vector3 scale3d; //0x0020
  char pad_002C[4]; //0x002C
}; //Size: 0x0030
static_assert(sizeof(FTransform) == 0x30);