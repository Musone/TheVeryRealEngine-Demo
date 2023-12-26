#pragma once

#ifdef TVRE_PLATFORM_WINDOWS
  #ifdef TVRE_BUILD_DLL
    #define TVRE_API __declspec(dllexport)
  #else
    #define TVRE_API __declspec(dllimport)
  #endif // TVRE_BUILD_DLL
#else
  #error The Very Real Engine only supports Windows.
#endif