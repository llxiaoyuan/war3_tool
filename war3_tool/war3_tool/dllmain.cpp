#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment (linker, "/export:ijlErrorStr=ijl15_old.ijlErrorStr,@6")
#pragma comment (linker, "/export:ijlFree=ijl15_old.ijlFree,@3")
#pragma comment (linker, "/export:ijlGetLibVersion=ijl15_old.ijlGetLibVersion,@1")
#pragma comment (linker, "/export:ijlInit=ijl15_old.ijlInit,@2")
#pragma comment (linker, "/export:ijlRead=ijl15_old.ijlRead,@4")
#pragma comment (linker, "/export:ijlWrite=ijl15_old.ijlWrite,@5")

/*
Game.dll+529D8 - 8B 32                 - mov esi,[edx]
Game.dll+529DA - 8B DE                 - mov ebx,esi
Game.dll+529DC - 89 75 F8              - mov [ebp-08],esi
*/

float* g_CameraDistanceAddress;
void __stdcall check_1650(int* address) {
    //44CE4000 00000000 42C80000 461C4000
    if (*address == 0x44CE4000) {//1650.0f
        //                                     100.0f                      10000.0f
        if (address[1] == 0x0 && address[2] == 0x42c80000 && address[3] == 0x461c4000) {
            if (g_CameraDistanceAddress != (float*)address) {
                InterlockedExchangePointer((void**)&g_CameraDistanceAddress, address);
                //g_CameraDistanceAddress = (float*)address;
                // 
                //printf("CameraDistanceAddress: %p CameraDistance: %f\n", g_CameraDistanceAddress, *g_CameraDistanceAddress);
                //for (size_t i = 0; i < 4; i++) {
                //    printf("%x ", address[i]);
                //}
                //printf("\n");
            }
        }
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

DWORD WINAPI InputFunction(LPVOID lpThreadParameter) {
    while (true) {
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
        Sleep(16);
    }
    return 0;
}

DWORD WINAPI ThreadFunction(LPVOID lpThreadParameter)
{
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);

    g_CameraDistanceAddress = NULL;

    CreateThread(NULL, 0, InputFunction, NULL, 0, NULL);

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

