// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <windows.h>

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

typedef BOOL(WINAPI* TrackingCountPtr)(void* a, char* b, int c, int d);

bool MMH55_LOADED = false;

EXTERN_DLL_EXPORT BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    {
        switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }
}

EXTERN_DLL_EXPORT BOOL TrackingCount(void* a, char* b, int c, int d) {
    //HINSTANCE UbiStatsDLL = LoadLibrary(TEXT("UbiStats2.dll"));
    //if (!UbiStatsDLL) {
    //    printf("Load failed with 0x%x\n", GetLastError());
    //    return FALSE;
    //}

    //TrackingCountPtr original_function = (TrackingCountPtr)GetProcAddress(UbiStatsDLL, "TrackingCount");
    //if (!original_function) {
    //    return FALSE;
    //}

    /****** MMH55_64 CODE START *******/
    /****** MMH55_64 CODE END *******/

    //BOOL result = original_function(a, b, c, d);
    //FreeLibrary(UbiStatsDLL);
   
    if (!MMH55_LOADED) {
        main();
        MMH55_LOADED = true;
    }

    return true;
}