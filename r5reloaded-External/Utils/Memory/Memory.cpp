#include "Memory.h"

bool Memory::Init()
{
    // Get pHandle
    Ghwnd = FindWindowA(NULL, "Apex Legends");
    if (!Ghwnd)
    {
        MessageBox(NULL, TEXT("Can't find R5Reloaded"), NULL, MB_ICONWARNING | MB_OK);
        return false;
    }
    GetWindowThreadProcessId(Ghwnd, &PID);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (!pHandle)
    {
        MessageBox(NULL, TEXT("Can't get process handle"), NULL, MB_ICONWARNING | MB_OK);
        return false;
    }

    // ModuleBase Address
    g.BaseAddress = GetModuleAddress("r5apex.exe");
    if (!g.BaseAddress)
    {
        MessageBox(NULL, TEXT("Can't get BaseAddress"), NULL, MB_ICONWARNING | MB_OK);
        return false;
    }

    return true;
}

uintptr_t Memory::GetModuleAddress(const std::string moduleName)
{
    MODULEENTRY32 entry = { };
    entry.dwSize = sizeof(MODULEENTRY32);

    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

    std::uintptr_t result = 0;

    while (Module32Next(snapShot, &entry))
    {
        if (!moduleName.compare(entry.szModule))
        {
            result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
            break;
        }
    }

    if (snapShot)
        CloseHandle(snapShot);

    return result;
}