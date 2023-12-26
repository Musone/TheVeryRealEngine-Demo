#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"

namespace TVRE {

  class TVRE_API Logger {
    static std::shared_ptr<spdlog::logger> core_logger_;
    static std::shared_ptr<spdlog::logger> client_logger_;

    public:
      static void Init();

      inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { 
        return core_logger_;
      }

      inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { 
        return client_logger_;
      }      
  };

}

// Core logging macros
#define TVRE_CORE_TRACE(...) ::TVRE::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define TVRE_CORE_INFO(...)  ::TVRE::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define TVRE_CORE_WARN(...)  ::TVRE::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define TVRE_CORE_ERROR(...) ::TVRE::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define TVRE_CORE_FATAL(...) ::TVRE::Logger::GetCoreLogger()->fatal(__VA_ARGS__)

// Client logging macros
#define TVRE_TRACE(...) ::TVRE::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define TVRE_INFO(...)  ::TVRE::Logger::GetClientLogger()->info(__VA_ARGS__)
#define TVRE_WARN(...)  ::TVRE::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define TVRE_ERROR(...) ::TVRE::Logger::GetClientLogger()->error(__VA_ARGS__)
#define TVRE_FATAL(...) ::TVRE::Logger::GetClientLogger()->fatal(__VA_ARGS__)
