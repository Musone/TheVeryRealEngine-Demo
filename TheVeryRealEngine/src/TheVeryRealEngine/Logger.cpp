#include "Logger.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace TVRE {

  std::shared_ptr<spdlog::logger> Logger::core_logger_;
  std::shared_ptr<spdlog::logger> Logger::client_logger_;

  void Logger::Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    
    core_logger_ = spdlog::stdout_color_mt("THE VERY REAL ENGINE");
    core_logger_->set_level(spdlog::level::trace);

    client_logger_ = spdlog::stdout_color_mt("APP");
    core_logger_->set_level(spdlog::level::trace);
  }

}