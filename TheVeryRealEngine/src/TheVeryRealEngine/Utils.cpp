#include <pch.h>

#include <stdexcept>

#include "Utils.h"

namespace TVRE {

  namespace Utils {

    VOID ThrowRuntimeError(const std::wstring message) {
      //MessageBox(0, message.c_str(), 0, 0);
      throw std::runtime_error("Runtime Error");
      //exit(1);
    };

  }

}