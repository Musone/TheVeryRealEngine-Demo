#pragma once

#include <string>
#include <functional> 

namespace TVRE {

  namespace ProcessIO {

    struct TVRE_API Module {
      HANDLE parent_process = 0;
      DWORD64 base_address = 0;
      DWORD64 size = 0;
    };

    HANDLE TVRE_API GetProcessHandle(const std::wstring& process_name);

    Module TVRE_API GetModule(const std::wstring& module_name, const HANDLE& process);

    DWORD64 TVRE_API ScanModuleForSignatureOffset(
      const Module& module, const BYTE* sig, const WCHAR* mask
    );

    void TVRE_API ReadBytes(
      const HANDLE& process, const DWORD64& address,
      const DWORD n_bytes, BYTE* buffer
    );

    void ThrowError(const std::wstring& errorMessage, const DWORD& pid = 0);

    template<typename T>
    T Read(const HANDLE& process, const DWORD64& address) {
      T buffer;
      ReadBytes(process, address, sizeof(T), (BYTE*)&buffer);

      return buffer;
    }

    template<typename T>
    void Write(const HANDLE& process, const DWORD64& address, const T& data) {
      if (address == 0) {
        ThrowError(
          L"You are dereferencing a nullptr: " +
          std::to_wstring((DWORD)address), GetProcessId(process)
        );
      }

      const BOOL failedToWrite =
        !WriteProcessMemory(
          process,
          (LPVOID)address,
          &data,
          sizeof(T),
          NULL
        );

      if (failedToWrite) {
        ThrowError(
          L"Failed to write memory location: " +
          std::to_wstring((DWORD)address), GetProcessId(process)
        );
      }
    }

    template<typename T>
    T ReadFromModule(const Module& module, const DWORD64& offset) {
      return Read<T>(module.parent_process, module.base_address + offset);
    }

    template<typename T>
    T WriteToModule(const Module& module, const DWORD64& offset, const T& data) {
      return Write<T>(module.parent_process, module.base_address + offset, data);
    }

  }

}