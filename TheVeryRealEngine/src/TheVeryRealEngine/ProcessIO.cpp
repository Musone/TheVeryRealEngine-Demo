#include <pch.h>

#include <TlHelp32.h> //Contains Read/Write Functions
#include <processthreadsapi.h> // Open process threads

#include "ProcessIO.h"

namespace TVRE {

	namespace ProcessIO {

		#pragma region Local forward declaration
		template<typename T>
		T getEntry(const std::wstring& queryEntryName,
							 const std::function <BOOL(T&)>& first,
							 const std::function <BOOL(T&)>& next,
							 const std::function <WCHAR* (T&)>& getName);

		bool sigEqual(const BYTE* data, const BYTE* sig, const WCHAR* mask);
		#pragma endregion

		#pragma region Public functions
		void ReadBytes(const HANDLE& process, const DWORD64& address, const DWORD nBytes, BYTE* buffer) {
			if (address == 0) {
				ThrowError(L"You are dereferencing a nullptr: " + std::to_wstring((DWORD)address), GetProcessId(process));
			}

			ZeroMemory(buffer, nBytes);
			const BOOL failedToRead = !ReadProcessMemory(process, (LPCVOID)address, buffer, nBytes, NULL);

			if (failedToRead) {
				ThrowError(L"Failed read memory location: " + std::to_wstring((DWORD)address), GetProcessId(process));
			}
		}

		HANDLE GetProcessHandle(const std::wstring& processName) {
			const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

			const auto processEntry = getEntry<PROCESSENTRY32>(
				processName,
				[&snapshot](PROCESSENTRY32& entry) -> BOOL { return Process32First(snapshot, &entry); },
				[&snapshot](PROCESSENTRY32& entry) -> BOOL { return Process32Next(snapshot, &entry); },
				[](PROCESSENTRY32& entry) ->WCHAR* { return entry.szExeFile; }
			);

			CloseHandle(snapshot);

			return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
		}


		Module GetModule(const std::wstring& moduleName, const HANDLE& process) {
			const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(process));

			const auto moduleEntry = getEntry<MODULEENTRY32>(
				moduleName,
				[&snapshot](MODULEENTRY32& entry) -> BOOL { return Module32First(snapshot, &entry); },
				[&snapshot](MODULEENTRY32& entry) -> BOOL { return Module32Next(snapshot, &entry); },
				[](MODULEENTRY32& entry) -> WCHAR* { return entry.szModule; }
			);

			CloseHandle(snapshot);

			return { process, DWORD64(moduleEntry.modBaseAddr), DWORD64(moduleEntry.modBaseSize) };
		}


		DWORD64 ScanModuleForSignatureOffset(const Module& module, const BYTE* sig, const WCHAR* mask) {
			MEMORY_BASIC_INFORMATION mbi = { 0 };
			DWORD64 offset = 0;

			while (offset < module.size) {
				VirtualQueryEx(module.parent_process, (LPCVOID)(module.base_address + offset), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
				if (mbi.State != MEM_FREE) {
					BYTE* buffer = new BYTE[mbi.RegionSize];

					const BOOL failedToRead = !ReadProcessMemory(module.parent_process, mbi.BaseAddress, buffer, mbi.RegionSize, NULL);

					if (failedToRead) {
						ThrowError(L"Failed to read memory location: " + std::to_wstring(offset));
					}

					for (SIZE_T i = 0; i < mbi.RegionSize; i++) {
						if (sigEqual(buffer + i, sig, mask)) {
							delete[] buffer;
							return offset + i;
						}
					}
					delete[] buffer;
				}
				offset += mbi.RegionSize;
			}

			const std::wstring errMessage = L"Could not find matching signature\nmask: " + std::wstring(mask); 
			Utils::ThrowRuntimeError(errMessage);
			return 0; // return is to make compiler happy, but ThrowRuntimeError should exit the program.
		}

		#pragma endregion

		#pragma region Local function definitions

		// Generic function for finding a Process/Module/Heap Entry by name.
		// Takes 3 functions as parameters that (ex. for a process) should respectively 
		// return: Process32First, Process32Next, processEntry.name
		template<typename T>
		T getEntry(const std::wstring& queryEntryName,
							 const std::function <BOOL(T&)>& first,
							 const std::function <BOOL(T&)>& next,
							 const std::function <WCHAR* (T&)>& getName) {

			T entry{ sizeof(T) };
			BOOL wasCopied = first(entry);
			BOOL matchFound = FALSE;

			while (wasCopied && !matchFound) {
				WCHAR* entryName = getName(entry);
				const BOOL isMatchingProcess = !wcscmp(entryName, queryEntryName.c_str());

				if (isMatchingProcess) {
					matchFound = TRUE;
				} else {
					wasCopied = next(entry);
				}
			}

			if (!matchFound) {
				ThrowError(L"Could not find an entry with a matching name: " + queryEntryName);
			}

			return entry;
		}


		void ThrowError(const std::wstring& errorMessage, const DWORD& pid) {
			const DWORD err = GetLastError();
			std::wstring message = errorMessage;

			if (pid) {
				message += L"\nProcess ID: " + std::to_wstring(pid);
			}
			message += L"\nError code: " + std::to_wstring(err);

			Utils::ThrowRuntimeError(message);
		}

		bool sigEqual(const BYTE* data, const BYTE* sig, const WCHAR* mask) {
			for (; *mask; mask++, sig++, data++) {
				if (*mask == L'x' && *data != *sig) {
					return false;
				}
			}
			return true;
		}
		#pragma endregion
	}

}