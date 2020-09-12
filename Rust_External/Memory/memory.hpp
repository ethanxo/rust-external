#pragma once

#include "..\includes.hpp"

#include "process.hpp"

class Memory {
private:
	HANDLE handle;
	int pid;

public:
	// "Attach" to a process (create handle to process)
	bool mem_attach(const WCHAR* proc_name) {
		// Get pid of process 
		this->pid = proc_pid(proc_name);

		// Check pid validity (not 0)
		if (!this->pid) return false;

		// Print out pid
		printf("[+] Process found at PID %d\n", this->pid);

		// Open a handle to the target process (w/ all possible access rights) and check validity
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, this->pid);

		// Check handle validity
		if (handle == INVALID_HANDLE_VALUE) return false;

		// Store the handle and return true
		this->handle = handle;
		return true;
	}

	// "Detach" from a process (close handle to process)
	bool mem_detach() {
		// Close handle, invalidate handle, return true
		CloseHandle(this->handle);
		this->handle = INVALID_HANDLE_VALUE;
		return true;
	}

	// Read type from address
	template<typename type>
	type mem_read(uint64_t address) {
		// Create buffer, read from addr to buffer, return buffer
		type buffer;
		ReadProcessMemory(this->handle, (LPVOID)address, &buffer, sizeof(buffer), 0);
		return buffer;
	}

	// Read overload for RPM-style calls
	bool mem_read(uint64_t address, LPVOID buffer, SIZE_T size)
	{
		// Read from addr to buffer
		if (ReadProcessMemory(this->handle, (LPCVOID)address, buffer, size, 0))
			return true;
		// Return false if RPM failed
		return false;
	}

	// Write type to address
	template<typename type>
	bool mem_write(uint64_t address, type value) {
		// Create buffer, set buffer content, calculator size of buffer, write to addr from buffer, return T/F
		type buffer = value;
		SIZE_T size = sizeof(buffer);
		return WriteProcessMemory(this->handle, (LPVOID)address, (LPVOID)buffer, size, 0);
	}

	// Read type from address + chain of ptrs
	template<typename type>
	type mem_readchain(uint64_t address, std::vector<uint64_t> chain) {
		// Create variable to store addr, iterate and read through chain vector, return final read
		uint64_t cur = address;
		for (int i = 0; i < chain.size() - 1; i++)
			cur = this->mem_read<uint64_t>(cur + chain[i]);
		return this->mem_read<type>(cur + chain[chain.size() - 1]); 
	}

	// Get base addr of module in the process
	uint64_t mem_modulebase(const WCHAR* mod_name) {
		// Instance of module entry for process snapshot
		MODULEENTRY32 modentry;
		modentry.dwSize = sizeof(MODULEENTRY32);

		// Instance of snapshot of process modules
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)this->pid);
		
		// Return 0 (not found) if snapshot is invalid
		if (snapshot == INVALID_HANDLE_VALUE) return 0;
		
		// Return 0 (not found) if first module in snapshot is null
		if (!Module32First(snapshot, &modentry)) return 0; 
		
		// Iterate through module list until target module is found (compare module name to mod_name)
		while (Module32Next(snapshot, &modentry))
			if (!wcscmp(modentry.szModule, mod_name))
				return (uint64_t)modentry.modBaseAddr;
		
		// Return 0 (not found) if there were no matches in the snapshot
		return 0;
	}

	// Scan for ptr to il2cpp class -- PASTED, IGNORE LACK OF EXPLANATION
	uint64_t mem_il2cppscan(const char* class_name)
	{
		uint64_t base = this->mem_modulebase(L"GameAssembly.dll");
		IMAGE_DOS_HEADER dos_header = this->mem_read<IMAGE_DOS_HEADER>(base);
		IMAGE_SECTION_HEADER data_header = this->mem_read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
		IMAGE_SECTION_HEADER next_section = this->mem_read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
		DWORD data_size = next_section.VirtualAddress - data_header.VirtualAddress;

		// Return 0 (not found) if section is not .data
		if (strcmp((char*)data_header.Name, ".data")) return 0;

		// Scan for the requested class
		for (uint64_t offset = data_size; offset > 0; offset -= 8)
		{
			char klass_name[0x100] = { 0 };
			auto klass = this->mem_read<uint64_t>(base + data_header.VirtualAddress + offset);
			if (!klass) continue;
			auto name_pointer = this->mem_read<uint64_t>(klass + 0x10);
			if (!name_pointer) continue;
			this->mem_read(name_pointer, klass_name, sizeof(klass_name));
			if (!strcmp(klass_name, class_name)) 
				return klass;
		}

		return 0;
	}
};

extern Memory* mem;