#include <TheVeryRealEngine.h>

#include "resources/SotOffsets.h"
#include "resources/SotStructs.h"
#include "resources/Gnames.h"
#include "Structs.h"

namespace SotHack {

  using namespace TVRE;

  #pragma region Signatures
  const BYTE u_world_sig[] = {
    BYTE(0x48),
    BYTE(0x8B),
    BYTE(0x05),
    BYTE(0xdeadbeef),
    BYTE(0xdeadbeef),
    BYTE(0xdeadbeef),
    BYTE(0xdeadbeef),
    BYTE(0x48),
    BYTE(0x8B),
    BYTE(0x88),
    BYTE(0xdeadbeef),
    BYTE(0xdeadbeef),
    BYTE(0xdeadbeef),
    BYTE(0xdeadbeef),
    BYTE(0x48),
    BYTE(0x85),
    BYTE(0xC9),
    BYTE(0x74),
    BYTE(0x06),
    BYTE(0x48),
    BYTE(0x8B),
    BYTE(0x49),
    BYTE(0x70),
  };

  const BYTE g_name_sig[] = {
    byte(0x48),
    byte(0x8B),
    byte(0x1D),
    byte(0xdeadbeef),
    byte(0xdeadbeef),
    byte(0xdeadbeef),
    byte(0xdeadbeef),
    byte(0x48),
    byte(0x85),
    byte(0xDB),
    byte(0x75),
    byte(0xdeadbeef),
    byte(0xB9),
    byte(0x08),
    byte(0x04),
    byte(0x00),
    byte(0x00)
  };
  #pragma endregion

  class SotHack : public TVRE::Application {
    TVRE::Graphics* graphics_ = nullptr;

    uint64_t ptr_g_name_ = 0;
    uint64_t camera_manager_ = 0;
    uint64_t ptr_athena_game_state_ = 0;
    uint64_t ptr_persistant_level_ = 0;

    std::unordered_map<uint32_t, std::wstring> g_name_cache_;

    AGameState athena_game_state_;
    std::wstring player_list_message_ = L"";
    std::vector<AActor> list_player_;
    std::vector<uint64_t> ptr_list_ship_scene_component_;
    std::vector<SceneComponentAndFName> level_actor_scene_components_;
    std::vector<uint64_t> ptr_list_skeletal_mesh_;

    // ~~~~~ start new Structure ~~~~~~~~~~~~
    std::vector<Crew> list_crew_;
    // ~~~~~ end new Structure ~~~~~~~~~~~~

    Camera camera_;
    ViewMatrix view_matrix_;
    Vector2 result_;

    bool show_all_actors_ = false;
    bool enable_aimbot_ = false;
    bool cap_framerate_ = true;
    bool dot_crosshair_ = false;
    double deltaTime_ = 0.0;
    std::wstring fps_message_;
    float auto_aim_fov_ = 350.0f;

  public:
    SotHack() {
      message_.message = WM_NULL;
    }

    ~SotHack() {}

    void Run() override {
      Init();
      graphics_ = TVRE::Graphics::GetInstance();
      TVRE::Timer timer;

      constexpr double SECONDS_PER_LIST_REFRESH = 10.0;
      constexpr double PLAYER_LIST_RESFRESH_RATE =
        (SECONDS_PER_LIST_REFRESH * 1000.0 * 1000.0);
      double time_of_last_player_list_refresh = -PLAYER_LIST_RESFRESH_RATE;

      constexpr double INPUT_REFRESH_RATE = 150000.0;
      double time_of_last_input = -INPUT_REFRESH_RATE;

      constexpr double ERROR_REFRESH_RATE = 1000000.0;
      double time_of_last_error = -ERROR_REFRESH_RATE;
      bool notify_hack_is_online = true;

      double time_of_last_graphics_refresh = 0;

      while (HandleWindowEvents()) {
        try {
          if (timer.GetTime() - time_of_last_input > INPUT_REFRESH_RATE) {
            if (GetAsyncKeyState(VK_NEXT)) {
              show_all_actors_ = !show_all_actors_;
              time_of_last_input = timer.GetTime();
            } else if (GetAsyncKeyState(VK_END)) {
              dot_crosshair_ = !dot_crosshair_;
              time_of_last_input = timer.GetTime();
            } else if (GetAsyncKeyState(VK_HOME)) {
              cap_framerate_ = !cap_framerate_;
              time_of_last_input = timer.GetTime();
            } else if (GetAsyncKeyState(VK_PRIOR)) {
              enable_aimbot_ = !enable_aimbot_;
              time_of_last_input = timer.GetTime();
            } else if (GetAsyncKeyState(107) && auto_aim_fov_ + 10.0f < 800.0f) {
              auto_aim_fov_ += 10.0f;
              time_of_last_input = timer.GetTime();
            } else if (GetAsyncKeyState(109) && auto_aim_fov_ - 10.0f > 0) {
              auto_aim_fov_ -= 10.0f;
              time_of_last_input = timer.GetTime();
            }
          }

          // Update entity list every 5 seconds.
          if (timer.GetTime() - time_of_last_player_list_refresh >
              PLAYER_LIST_RESFRESH_RATE) {

            time_of_last_player_list_refresh = timer.GetTime();

            athena_game_state_ =
              ProcessIO::Read<AGameState>(process_, ptr_athena_game_state_);

            UpdatePlayerList();
            UpdateShipList();
            UpdateULevelActors();
          }

          UpdateCamera();
          view_matrix_.SetViewMatrix(camera_.angle);

          graphics_->BeginDraw();
          graphics_->ClearScreen();

          DrawULevelActors();
          DrawPlayerList();
          DrawShipList();

          // TODO: Figure out how to render player skeletal meshes.
          DrawSkeletons();

          deltaTime_ = timer.GetTime() - time_of_last_graphics_refresh;
          time_of_last_graphics_refresh = timer.GetTime();

          DrawHUD();

          graphics_->EndDraw();

          if (enable_aimbot_ &&
              GetAsyncKeyState(VK_RBUTTON) /*&&
              timer.GetTime() - time_of_last_input > INPUT_REFRESH_RATE*/) {
            AutoAim();
            time_of_last_input = timer.GetTime();
          }

          if (notify_hack_is_online) {
            notify_hack_is_online = false;
            TVRE_INFO("SoT Hack online");
          }

        } catch (std::runtime_error& e) {

          if (timer.GetTime() - time_of_last_error < ERROR_REFRESH_RATE) {
            timer.Sleep(ERROR_REFRESH_RATE);
          }
          time_of_last_error = timer.GetTime();
          notify_hack_is_online = true;

          TVRE_ERROR("SoT Hack offline");

          if (graphics_->IsDrawing()) {
            graphics_->EndDraw();
          }

          time_of_last_player_list_refresh = -PLAYER_LIST_RESFRESH_RATE;
          time_of_last_input = -INPUT_REFRESH_RATE;
          Init();
        }

        if (cap_framerate_) {
          timer.Sleep(7.2);
        }
      }
    }

    #pragma region private
  private:

    inline float GetWindowWidth() {
      return window_rect_.right - window_rect_.left;
    }

    inline float GetWindowHeight() {
      return window_rect_.bottom - window_rect_.top;
    }

    const std::vector<std::wstring> TEAM_MATE_NAMES = {
      L"the_best_player3213",
      L"sweaty_gamer_223",
      L"dark_lord_19281"
    };
    void AutoAim() {
      bool found_a_target = false;
      Vector2 target_pos;
      float dist_of_closest_player = 1000000.0f;

      for (const auto& ptr_target : list_player_) {

        // TODO: Remove this when we start using our own objects to store player 
        // TODO: data.
        bool is_team_member = false;
        if (ptr_target.PlayerName) {

          const WStringBuffer playe_name_buffer =
            ProcessIO::Read<WStringBuffer>(
              process_,
              ptr_target.PlayerName
              );

          for (const auto& team_member_name : TEAM_MATE_NAMES) {
            if (team_member_name.compare(std::wstring(playe_name_buffer.data)) == 0) {
              is_team_member = true;
              break;
            };
          }
        }
        // TODO: ~~~~ end todo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (!ptr_target.RepAttachment || is_team_member) {
          continue;
        }
        auto target =
          ProcessIO::Read<USceneComponent>(process_, ptr_target.RepAttachment);

        target.absCoord1.z += 15.0f;
        w2s(target.absCoord1, result_);

        Vector3 vec3_from_local_player = target.absCoord1 - camera_.pos;
        Vector2 vec2_to_target = result_ - Vector2(screen_center_x_, screen_center_y_);
        if (vec2_to_target.Length() <= auto_aim_fov_ &&
            vec3_from_local_player.Length() < dist_of_closest_player) {
          dist_of_closest_player = vec3_from_local_player.Length();
          found_a_target = true;
          target_pos = vec2_to_target;
        }
      }

      if (found_a_target) {
        float dx = target_pos.x / 3.0f;
        float dy = target_pos.y / 3.0f;

        constexpr float MAX_MOUSE_MOVEMENT = 70.0f;

        while (dx > MAX_MOUSE_MOVEMENT || dx < -MAX_MOUSE_MOVEMENT) {
          dx /= 5.0f;
        }

        while (dy > MAX_MOUSE_MOVEMENT || dy < -MAX_MOUSE_MOVEMENT) {
          dy /= 5.0f;
        }

        if (dx > MAX_MOUSE_MOVEMENT ||
            dy > MAX_MOUSE_MOVEMENT ||
            dx < -MAX_MOUSE_MOVEMENT ||
            dy < -MAX_MOUSE_MOVEMENT) {
          TVRE_ERROR("Auto Aim out of bounds error");
          throw std::runtime_error("Auto Aim out of bounds error");
        }

        mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
      }
    }

    void DrawHUD() {
      graphics_->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
      graphics_->DrawText(player_list_message_);

      graphics_->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
      if (dot_crosshair_) {
        graphics_->DrawPoint(screen_center_x_, screen_center_y_, 5.0f);
      } else {
        constexpr float CROSSHAIR_LENGTH = 10.0f;
        graphics_->DrawLine(
          screen_center_x_ - CROSSHAIR_LENGTH,
          screen_center_y_,
          screen_center_x_ + CROSSHAIR_LENGTH,
          screen_center_y_,
          2.0f
        );
        graphics_->DrawLine(
          screen_center_x_,
          screen_center_y_ - CROSSHAIR_LENGTH,
          screen_center_x_,
          screen_center_y_ + CROSSHAIR_LENGTH,
          2.0f
        );
      }

      if (enable_aimbot_) {
        graphics_->DrawCircle(
          screen_center_x_,
          screen_center_y_,
          auto_aim_fov_,
          1.0f
        );
      }

      fps_message_ =
        std::to_wstring(uint32_t(1000000.0 / deltaTime_)) + L"   FPS";

      graphics_->DrawText(fps_message_, 100, 0, 100);
    }

    void DrawShipList() {
      graphics_->SetColor(1.f, 0.f, 0.f, 1.f);

      for (const auto& ptr_ship : ptr_list_ship_scene_component_) {
        USceneComponent ship = ProcessIO::Read<USceneComponent>(process_, ptr_ship);
        if ((camera_.pos - ship.absCoord1).Length() > 1500.0f) {
          DrawCube(ship.absCoord1, ship.RelativeRotation, 1500.0f, 700.0f, 700.0f, 3.0f);
        }
      }
    }

    void DrawPlayerList() {
      graphics_->SetColor(1.f, 1.f, 0.f, 1.f);

      for (const auto& player : list_player_) {
        USceneComponent target;

        if (!player.Ping && player.RepAttachment) {
          target = ProcessIO::Read<USceneComponent>(process_, player.RepAttachment);
          //Cube::Draw(graphics_, target.absCoord1, target.RelativeRotation, local_coords_, v_axis_x_, v_axis_y_, v_axis_z_, 50.f, 50.f, 100.f, screen_center_x_, screen_center_y_, 1.f, fov_);
          
          // TODO: refactor code to display more useful information.
          /*if (player.PlayerName) {

          }*/
          
          DrawCube(target.absCoord1, target.RelativeRotation, 50.0f, 50.0f, 100.0f, 1.0f);
        }

        target = ProcessIO::Read<USceneComponent>(process_, player.USceneComponent);
        w2s(target.absCoord1, result_);
      }
    }

    void DrawULevelActors() {
      graphics_->SetColor(1.f, 0.f, 1.f, 1.f);

      for (const SceneComponentAndFName& thing : level_actor_scene_components_) {

        if (!show_all_actors_ && !IsInterestingActor(thing.name)) {
          continue;
        }

        const USceneComponent sc =
          ProcessIO::Read<USceneComponent>(process_, thing.ptr_scene_component);

        w2s(sc.absCoord1, result_);

        if (IsOnScreen(result_)) {
          graphics_->DrawPoint(result_.x, result_.y, 5.f);
          graphics_->DrawText(thing.name, result_.x, result_.y, 300);
        }
      }
    }

    void DrawSkeletons() {
      for (const auto& ptr_skeletal_mesh : ptr_list_skeletal_mesh_) {

        constexpr uint64_t OFFSET_POS_1 = 0x12c;
        const Vector3 pos_1 =
          ProcessIO::Read<Vector3>(
            process_,
            ptr_skeletal_mesh + OFFSET_POS_1
            );
        //w2s(pos_1, result_);
        //graphics_->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        //graphics_->DrawCircle(result_.x, result_.y, 20.0f, 5.0f);

        constexpr uint64_t OFFSET_POS_2 = 0x160;
        const Vector3 pos_2 =
          ProcessIO::Read<Vector3>(
            process_,
            ptr_skeletal_mesh + OFFSET_POS_2
            );
        //w2s(pos_2, result_);
        //graphics_->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        //graphics_->DrawCircle(result_.x, result_.y, 20.0f, 5.0f);

        constexpr uint64_t OFFSET_POS_3 = 0x1b0;
        const Vector3 pos_3 =
          ProcessIO::Read<Vector3>(
            process_,
            ptr_skeletal_mesh + OFFSET_POS_3
            );
        //w2s(pos_3, result_);
        //graphics_->SetColor(1.0f, 5.0f, 2.0f, 1.0f);
        //graphics_->DrawCircle(result_.x, result_.y, 20.0f, 5.0f);

        constexpr uint64_t OFFSET_CACHED_SPACE_BASES = 0x758;
        const TArray list_cached_space_bases = ProcessIO::Read<TArray>(
          process_,
          ptr_skeletal_mesh + OFFSET_CACHED_SPACE_BASES
          );

        

        const Vector3 delete_me(0, 0, 0);
        graphics_->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        DrawCube(pos_1, delete_me, 50.0f, 50.0f, 100.0f, 1.0f);
        graphics_->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        DrawCube(pos_2, delete_me, 50.0f, 50.0f, 100.0f, 1.0f);
        graphics_->SetColor(1.0f, 5.0f, 2.0f, 1.0f);
        DrawCube(pos_3, delete_me, 50.0f, 50.0f, 100.0f, 1.0f);
      }
    }

    bool IsOnScreen(const Vector2& point) {
      return window_rect_.left < point.x&&
        point.x < window_rect_.right&&
        window_rect_.top < point.y&&
        point.y < window_rect_.bottom;
    }

    inline void w2s(const Vector3& target_pos, Vector2& out) {
      view_matrix_.WorldToScreen(
        screen_center_x_,
        screen_center_y_,
        camera_.fov,
        target_pos,
        camera_.pos,
        out
      );
    }

    void UpdateShipList() {
      TArray crewed_ship_list =
        ProcessIO::Read<TArray>(
          process_,
          athena_game_state_.ShipService + 0x410
          );

      constexpr uint32_t SHIP_LIST_BUFFER_LENGTH = 8;
      CrewShipEntry crewed_ship_entry_buffer[SHIP_LIST_BUFFER_LENGTH];

      ProcessIO::ReadBytes(
        process_,
        crewed_ship_list.ptr_base,
        SHIP_LIST_BUFFER_LENGTH * sizeof(crewed_ship_entry_buffer[0]),
        (uint8_t*)&crewed_ship_entry_buffer
      );

      ptr_list_ship_scene_component_.clear();

      for (size_t i = 0; i < crewed_ship_list.size; ++i) {
        if (crewed_ship_entry_buffer[i].ptr_ship) {
          ptr_list_ship_scene_component_.push_back(
            ProcessIO::Read<AActor>(
              process_,
              crewed_ship_entry_buffer[i].ptr_ship).USceneComponent
          );
        }
      }
    }

    void UpdateCamera() {
      constexpr uint32_t CAMERA_MANAGER_CACHE_OFFSET = 0x450;
      const SotCamera camera = ProcessIO::Read<SotCamera>(process_, camera_manager_ + CAMERA_MANAGER_CACHE_OFFSET);
      camera_.angle = camera.angle;
      camera_.pos = camera.pos;
      camera_.fov = camera.fov;
    }

    void UpdateULevelActors() {
      level_actor_scene_components_.clear();
      ptr_list_skeletal_mesh_.clear();
      //player_skeletons_.clear();
      const TArray u_level_actor_list =
        ProcessIO::Read<TArray>(process_, ptr_persistant_level_ + 0xa0);

      for (size_t i = 0; i < u_level_actor_list.size; ++i) {
        const DWORD64 ptr_actor =
          ProcessIO::Read<DWORD64>(
            process_,
            u_level_actor_list.ptr_base + i * sizeof(DWORD64)
            );

        if (ptr_actor) {
          const AActor actor = ProcessIO::Read<AActor>(process_, ptr_actor);

          if (actor.USceneComponent && actor.fNameIndex) {
            SceneComponentAndFName temp;

            temp.ptr_scene_component = actor.USceneComponent;
            temp.name = GetGName(actor.fNameIndex);

            level_actor_scene_components_.push_back(temp);

            if (temp.name.compare(L"BP_PlayerPirate_C") == 0) {
              constexpr uint64_t OFFSET_BP_PLAYERPIRATE_C_CURRENTSKELETALMESH = 0x440;
              const uint64_t ptr_skeletal_mesh_component =
                ProcessIO::Read<uint64_t>(
                  process_,
                  ptr_actor + OFFSET_BP_PLAYERPIRATE_C_CURRENTSKELETALMESH
                  );

              if (ptr_skeletal_mesh_component) {
                ptr_list_skeletal_mesh_.push_back(ptr_skeletal_mesh_component);
                //constexpr uint64_t OFFSET_SKELETALMESH_SKELETON = 0x50;
                //constexpr uint64_t OFFSET_POS_1 = 0x160;
                /*const Vector3 ptr_skeleton =
                  ProcessIO::Read<Vector3>(
                    process_,
                    ptr_skeletal_mesh_component + OFFSET_POS_1
                    );
                    uint32_t delet_me = 0;*/
                    //player_skeletons_.push_back(ptr_skeleton);
              }
            }

          }
        }
      }
    }

    void UpdatePlayerList() {
      constexpr uint32_t BUFFER_SIZE = 15;
      uint64_t player_state_ptr_buffer[BUFFER_SIZE];

      ProcessIO::ReadBytes(
        process_,
        athena_game_state_.player_list.ptr_base,
        BUFFER_SIZE * sizeof(player_state_ptr_buffer[0]),
        (uint8_t*)&player_state_ptr_buffer[0]
      );

      list_player_.clear();

      for (size_t i = 0; i < athena_game_state_.player_list.size; ++i) {
        list_player_.push_back(
          ProcessIO::Read<AActor>(process_, player_state_ptr_buffer[i])
        );
      }

      UpdatePlayerListMessage();
    }

    void UpdatePlayerListMessage() {
      player_list_message_ =
        std::to_wstring(athena_game_state_.player_list.size) +
        L" Players online\n";

      uint32_t count = 1;
      for (size_t i = 0; i < athena_game_state_.player_list.size; ++i) {
        if (list_player_[i].PlayerNameLength > 0) {
          player_list_message_ += std::to_wstring(count) + L": ";
          const WStringBuffer name = ProcessIO::Read<WStringBuffer>(process_, list_player_[i].PlayerName);
          player_list_message_ += std::wstring(name.data) + L'\n';
          ++count;
        }
      }
    }

    inline bool HandleWindowEvents() {
      if (PeekMessage(&message_, NULL, 0, 0, PM_REMOVE)) {
        DispatchMessage(&message_);
      }

      return true;
    }

    bool IsInterestingActor(const std::wstring& actor_name) {
      return names.find(actor_name) != names.end();
    }

    std::wstring GetGName(const uint32_t& f_name_index) {
      if (g_name_cache_.find(f_name_index) == g_name_cache_.end()) {
        auto offset1 = 0x8 * (f_name_index / 0x4000);
        auto offset2 = 0x8 * (f_name_index % 0x4000);
        uint64_t level1 = ProcessIO::Read<uint64_t>(process_, ptr_g_name_ + offset1);
        uint64_t level2 = ProcessIO::Read<uint64_t>(process_, level1 + offset2);
        StringBuffer result = ProcessIO::Read<StringBuffer>(process_, level2 + 0x10);

        std::string temp = std::string(result.data);
        std::wstring name = std::wstring(temp.begin(), temp.end());

        g_name_cache_[f_name_index] = name;
      }

      return g_name_cache_[f_name_index];
    }

    void Init() {
      ProcessIO::Module game = ProcessIO::GetModule(L"SoTGame.exe", process_);

      #pragma region GName Setup
      const uint64_t g_name_offset = ProcessIO::ScanModuleForSignatureOffset(
        game,
        g_name_sig,
        L"xxx????xxxx?xxxxx"
      );

      const uint64_t g_name_base = game.base_address + g_name_offset;

      const uint32_t ptr_g_name_offset =
        ProcessIO::Read<uint32_t>(
          process_,
          g_name_base + 3
          );

      ptr_g_name_ =
        ProcessIO::Read<uint64_t>(
          process_,
          g_name_base + ptr_g_name_offset + 7
          );
      #pragma endregion

      #pragma region UWorld Setup
      const uint64_t u_world_offset = ProcessIO::ScanModuleForSignatureOffset(
        game,
        u_world_sig,
        L"xxx????xxx????xxxxxxxxx"
      );

      const uint64_t u_world_base = game.base_address + u_world_offset;

      const uint32_t u_world_ptr_world_address_offset =
        ProcessIO::Read<uint32_t>(
          process_,
          u_world_base + 3
          );

      const uint64_t ptr_world =
        ProcessIO::Read<uint64_t>(
          process_,
          u_world_base + u_world_ptr_world_address_offset + 7
          );

      #define WORLD_ADDRESS_PTR_GAME_INSTANCE_OFFSET 0x1C0
      const uint64_t game_instance_base =
        ProcessIO::Read<uint64_t>(
          process_,
          ptr_world + WORLD_ADDRESS_PTR_GAME_INSTANCE_OFFSET
          );

      #define GAME_INSTANCE_PTR_PLAYER_LIST_OFFSET 0x38
      const uint64_t player_list_base = ProcessIO::Read<uint64_t>(
        process_,
        game_instance_base + GAME_INSTANCE_PTR_PLAYER_LIST_OFFSET
        );

      const uint64_t u_player_base =
        ProcessIO::Read<uint64_t>(
          process_,
          player_list_base + 0 * sizeof(uint64_t)
          );

      #define PLAYER_CONTROLLER_OFFSET 0x30
      const uint64_t playerController =
        ProcessIO::Read<uint64_t>(
          process_,
          u_player_base + PLAYER_CONTROLLER_OFFSET
          );

      #define CAMERA_MANAGER_OFFSET 0x458
      camera_manager_ = ProcessIO::Read<uint64_t>(
        process_,
        playerController + CAMERA_MANAGER_OFFSET
        );

      #define GAME_STATE_OFFSET 0x58
      ptr_athena_game_state_ =
        ProcessIO::Read<uint64_t>(
          process_,
          ptr_world + GAME_STATE_OFFSET
          );

      #define PERSISTANT_LEVEL_OFFSET 0x30
      ptr_persistant_level_ =
        ProcessIO::Read<uint64_t>(
          process_,
          ptr_world + PERSISTANT_LEVEL_OFFSET
          );
      #pragma endregion
    }

    void RotateAndTranslate(
      Vector3& point, const Vector3& world_position,
      const float& sin_t, const float& cos_t
    ) {
      const float x = point.x, y = point.y;
      point.x = world_position.x + (x * cos_t - y * sin_t);
      point.y = world_position.y + (x * sin_t + y * cos_t);
      point.z = world_position.z + point.z;
    }

    void DrawCube(
      const Vector3& target_pos, const Vector3& target_rotation,
      const float& length, const float& width, const float& height,
      const float& thickness
    ) {

      const float sin_t = sinf(DEG2RAD(target_rotation.y));
      const float cos_t = cosf(DEG2RAD(target_rotation.y));

      Vector2 vertex1, vertex2, vertex3, vertex4, vertex5, vertex6, vertex7, vertex8;

      Vector3 ftl(0.f, 0.f, 0.f);
      ftl.x -= length;
      ftl.y += width;
      ftl.z += height;

      Vector3 fbr = ftl;
      fbr.x += 2.f * length;
      fbr.z -= 2.f * height;

      Vector3 btl = ftl;
      btl.y -= 2.f * width;

      Vector3 bbr = fbr;
      bbr.y -= 2.f * width;

      Vector3 ftr = Vector3(fbr.x, fbr.y, ftl.z);
      Vector3 fbl = Vector3(ftl.x, ftl.y, fbr.z);
      Vector3 btr = Vector3(bbr.x, bbr.y, btl.z);
      Vector3 bbl = Vector3(btl.x, btl.y, bbr.z);

      RotateAndTranslate(ftl, target_pos, sin_t, cos_t);
      RotateAndTranslate(ftr, target_pos, sin_t, cos_t);
      RotateAndTranslate(fbr, target_pos, sin_t, cos_t);
      RotateAndTranslate(fbl, target_pos, sin_t, cos_t);
      RotateAndTranslate(btl, target_pos, sin_t, cos_t);
      RotateAndTranslate(btr, target_pos, sin_t, cos_t);
      RotateAndTranslate(bbr, target_pos, sin_t, cos_t);
      RotateAndTranslate(bbl, target_pos, sin_t, cos_t);

      w2s(ftl, vertex1);
      w2s(ftr, vertex2);
      w2s(fbr, vertex3);
      w2s(fbl, vertex4);
      w2s(btl, vertex5);
      w2s(btr, vertex6);
      w2s(bbr, vertex7);
      w2s(bbl, vertex8);

      if (IsOnScreen(vertex1) ||
          IsOnScreen(vertex2) ||
          IsOnScreen(vertex3) ||
          IsOnScreen(vertex4) ||
          IsOnScreen(vertex5) ||
          IsOnScreen(vertex6) ||
          IsOnScreen(vertex7) ||
          IsOnScreen(vertex8)) {

        graphics_->DrawLine(vertex1.x, vertex1.y, vertex2.x, vertex2.y, thickness);
        graphics_->DrawLine(vertex1.x, vertex1.y, vertex4.x, vertex4.y, thickness);
        graphics_->DrawLine(vertex2.x, vertex2.y, vertex3.x, vertex3.y, thickness);
        graphics_->DrawLine(vertex4.x, vertex4.y, vertex3.x, vertex3.y, thickness);

        graphics_->DrawLine(vertex5.x, vertex5.y, vertex6.x, vertex6.y, thickness);
        graphics_->DrawLine(vertex5.x, vertex5.y, vertex8.x, vertex8.y, thickness);
        graphics_->DrawLine(vertex6.x, vertex6.y, vertex7.x, vertex7.y, thickness);
        graphics_->DrawLine(vertex8.x, vertex8.y, vertex7.x, vertex7.y, thickness);

        graphics_->DrawLine(vertex1.x, vertex1.y, vertex5.x, vertex5.y, thickness);
        graphics_->DrawLine(vertex2.x, vertex2.y, vertex6.x, vertex6.y, thickness);
        graphics_->DrawLine(vertex3.x, vertex3.y, vertex7.x, vertex7.y, thickness);
        graphics_->DrawLine(vertex4.x, vertex4.y, vertex8.x, vertex8.y, thickness);
      }
    }

    #pragma endregion
  };

}

TVRE::Application* TVRE::CreateApplication() {
  TVRE::Application* app = new SotHack::SotHack();
  app->game_window_name = L"Sea of Thieves";
  app->process_name = L"SoTGame.exe";

  return app;
}
