#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//#pragma comment(linker, "/EXPORT:ijlGetLibVersion=ijl15_old.ijlGetLibVersion")
//#pragma comment(linker, "/EXPORT:ijlInit=ijl15_old.ijlInit")
//#pragma comment(linker, "/EXPORT:ijlFree=ijl15_old.ijlFree")
//#pragma comment(linker, "/EXPORT:ijlRead=ijl15_old.ijlRead")
//#pragma comment(linker, "/EXPORT:ijlWrite=ijl15_old.ijlWrite")
//#pragma comment(linker, "/EXPORT:ijlErrorStr=ijl15_old.ijlErrorStr")

/*
Game.dll+529D8 - 8B 32                 - mov esi,[edx]
Game.dll+529DA - 8B DE                 - mov ebx,esi
Game.dll+529DC - 89 75 F8              - mov [ebp-08],esi
*/

float* g_CameraDistanceAddress;
void __stdcall check_1650(int* address) {
    if (g_CameraDistanceAddress != NULL) {
        if (GetAsyncKeyState(VK_OEM_MINUS) & 1) {
            *g_CameraDistanceAddress -= 100.0f;
            //printf("CameraDistanceAddress: %p CameraDistance: %f\n", g_CameraDistanceAddress, *g_CameraDistanceAddress);
        }
        else if (GetAsyncKeyState(VK_OEM_PLUS) & 1) {
            *g_CameraDistanceAddress += 100.0f;
            //printf("CameraDistanceAddress: %p CameraDistance: %f\n", g_CameraDistanceAddress, *g_CameraDistanceAddress);
        }
    }
    if (*address == 0x44CE4000) { //1650.0f
        g_CameraDistanceAddress = (float*)address;
    }
}

extern "C"
{
    uintptr_t func_check_1650;
    uintptr_t target_next;
    void sub_hook();
}

void Write_jmp_E9(uint8_t* address, uint32_t displacement) {
    *address = 0xE9;
    address++;
    *(uint32_t*)address = displacement;
}
void Write_jmp_E9(uint8_t* address, uint32_t source, uint32_t destination) {
    Write_jmp_E9(address, destination - (source + 5));
}

DWORD WINAPI ThreadFunction(LPVOID lpThreadParameter)
{
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
    g_CameraDistanceAddress = NULL;

    uint8_t* Game_dll;
    while (true) {
        Game_dll = (uint8_t*)GetModuleHandleW(L"Game.dll");
        if (Game_dll != NULL) {
            break;
        }
        Sleep(1000);
    }

    uint8_t* target = Game_dll + 0x529D8;

    //printf("target: %p\n", target);

    func_check_1650 = (uintptr_t)check_1650;
    target_next = (uintptr_t)target + 7;

    DWORD Protect = PAGE_EXECUTE_READWRITE;
    VirtualProtect(target, 5, Protect, &Protect);
    Write_jmp_E9(target, (uint32_t)target, (uint32_t)sub_hook);
    VirtualProtect(target, 5, Protect, &Protect);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

