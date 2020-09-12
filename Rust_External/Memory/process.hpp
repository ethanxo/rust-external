#pragma once

#include "..\includes.hpp"

// Get pid of a process by its executable name
inline int proc_pid(const WCHAR* proc_name) {
    // Instance of entry for process snapshot
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // Instance of snapshot
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    // Return 0 (not found) if snapshot is invalid
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    // Return 0 (not found) if first process in snapshot is null
    if (!Process32First(snapshot, &entry)) return 0;

    // Iterate through process list until the target process is found (compare executable name to proc_name)
    while (Process32Next(snapshot, &entry))
        if (!wcscmp(entry.szExeFile, proc_name)) 
            return (int)entry.th32ProcessID;

    // Return 0 (not found) if there were no matches in the snapshot
    return 0;
}

// Kill a process by its pid
inline void proc_kill(DWORD pid) {
    // Open handle to process, terminate
    HANDLE handle;
    handle = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
    TerminateProcess(handle, 0);
}