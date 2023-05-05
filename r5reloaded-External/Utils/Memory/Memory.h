#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "../global.h"
#include "../SimpleMath\SimpleMath.h"
using namespace DirectX::SimpleMath;

class Memory 
{
public:
    HWND Ghwnd;
    DWORD PID;
    HANDLE pHandle;

    uintptr_t GetModuleAddress(const std::string moduleName);
	bool Init();

	template <typename T>
	constexpr const T Read(const std::uintptr_t& address) const noexcept
	{
		T value = { };
		ReadProcessMemory(pHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	constexpr void Write(const std::uintptr_t& address, const T& value) const noexcept
	{
		WriteProcessMemory(pHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}
};