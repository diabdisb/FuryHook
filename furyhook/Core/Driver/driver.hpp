#pragma once
#define NOMINMAX 
#include "struct.hpp"

#include <Windows.h>
#undef min
#undef max
#include <TlHelp32.h>
#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include "../Overlay/skycrypt.hpp"

namespace driver
{

	bool SetWindowAffinity(uintptr_t HWND, UINT32 value);

	namespace detail
	{
		inline void* function;
		inline std::string process;
		inline DWORD pid;
		inline std::uint64_t base;
		inline DWORD size_of_image;
	}

	bool SetWindowAffinity(uintptr_t HWND, uint32_t value);

	bool running();
	bool load();
	ULONG64 GetDirBase();
	bool attach(const char* process_name);
	bool read(std::uint64_t dst, void* src, ULONGLONG size);
	bool read_virtual(std::uint64_t dst, void* src, ULONGLONG size);
	bool writevirtual(std::uint64_t dst, void* src, ULONGLONG size);
	bool write(std::uint64_t dst, void* src, ULONGLONG size);
	std::string read_str(std::uint64_t dst);
	bool Mouse_Event(long x, long y, unsigned short flag);

	bool Mouse_innit();

	template<typename T>
	inline T read(std::uint64_t dst)
	{
		if (!dst)
			return T();

		T buffer{};
		if (read(dst, &buffer, sizeof(buffer)))
			return buffer;
		else
			return T();
	}
	template <typename T>
	T readchain(const std::uint64_t address, std::vector<uint64_t> chain)
	{
		uint64_t cur_read = address;

		for (int i = 0; i < chain.size() - 1; ++i)
		{
			cur_read = read<uint64_t>(cur_read + chain[i]);
			if (!cur_read)
				return T();
		}

		return read<T>(cur_read + chain[chain.size() - 1]);
	}
	inline bool readsize(uintptr_t source, void* destination, int size)
	{
		bool STATUS = read(source, destination, size);

		return STATUS;
	}
	struct monostr
	{
		char buffer[128];
	};
	inline std::string ReadChar(uint64_t addr) {
		std::string str = read<monostr>(addr).buffer;
		if (!str.empty())
			return str;
		else
			return (std::string)_("31");
	}
	inline std::wstring read_wstring(uint64_t String_address, SIZE_T size, bool esp_driver = false)
	{
		const auto buffer = std::make_unique<wchar_t[]>(size);
		read(String_address, buffer.get(), size * 2);
		return std::wstring(buffer.get());
	}
	template<typename T>
	inline T read2(std::uint64_t dst)
	{
		if (!dst)
			return T();

		T buffer{};
		if (read_virtual(dst, &buffer, sizeof(buffer)))
			return buffer;
		else
			return T();
	}
	template<typename T>
	inline bool write(std::uint64_t dst, T buffer)
	{
		if (!dst)
			return false;

		write(dst, &buffer, sizeof(buffer));
		return TRUE;
	}
	template<typename T>
	inline bool write2(std::uint64_t dst, T buffer)
	{
		if (!dst)
			return false;

		writevirtual(dst, &buffer, sizeof(buffer));
		return TRUE;
	}

	std::uint64_t module(LPCWSTR module_name);
	bool valid(std::uint64_t dst);
	std::uint64_t get_process_exe_base();
	std::uint64_t get_peb();

	inline std::uint64_t GetModuleByName(std::uint64_t peb_address, const wchar_t* mod_name) {
		// Read PEB
		PEB peb{};
		read(peb_address, &peb, sizeof(peb));

		// Read Ldr
		PEB_LDR_DATA ldr{};
		read((std::uint64_t)peb.Ldr, &ldr, sizeof(ldr));

		// Get the head of the InMemoryOrderModuleList
		LIST_ENTRY* head = &ldr.InMemoryOrderModuleList;

		// Get address of first entry in the linked list
		LIST_ENTRY current{};
		read((std::uint64_t)head->Flink, &current, sizeof(current));

		// Pointer to store the base address if we find the requested module
		std::uint64_t found_module = 0;

		wprintf(L"[+] Beginning module enumeration:\n");

		// Iterate through the linked list
		while ((std::uint64_t)current.Flink != (std::uint64_t)head) {
			// Calculate the address of the LDR_DATA_TABLE_ENTRY containing this LIST_ENTRY
			std::uint64_t entry_addr = (std::uint64_t)current.Flink - offsetof(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			// Read the entry
			LDR_DATA_TABLE_ENTRY entry{};
			read(entry_addr, &entry, sizeof(entry));

			// Read module name
			wchar_t name[260] = {};
			USHORT len = entry.BaseDllName.Length;
			if (len > sizeof(name) - sizeof(wchar_t))
				len = sizeof(name) - sizeof(wchar_t);

			// Make sure we have a valid buffer to read from
			if (entry.BaseDllName.Buffer != nullptr && len > 0) {
				read((std::uint64_t)entry.BaseDllName.Buffer, name, len);
				name[len / sizeof(wchar_t)] = L'\0';

				//wprintf(L"    [+] Module: %ws (Base: 0x%llx)\n", name, (std::uint64_t)entry.DllBase);

				// If this is the module we're looking for, save its base address
				if (mod_name && _wcsicmp(name, mod_name) == 0) {
					found_module = (std::uint64_t)entry.DllBase;
					return found_module;
					
				}
			}
			else {
				//wprintf(L"    [!] Found module with invalid name buffer\n");
			}

			// Move to the next entry in the list
			read((std::uint64_t)current.Flink, &current, sizeof(current));
		}

		wprintf(L"[+] Module enumeration complete\n");

		// Return the found module's base address or 0 if not found
		return found_module;
	}

	inline // Alternative implementation if you want a dedicated function just to print all modules
		void PrintAllModules(std::uint64_t peb_address) {
		// Read PEB
		PEB peb{};
		read(peb_address, &peb, sizeof(peb));

		// Read Ldr
		PEB_LDR_DATA ldr{};
		read((std::uint64_t)peb.Ldr, &ldr, sizeof(ldr));

		// Get the head of the InMemoryOrderModuleList
		LIST_ENTRY* head = &ldr.InMemoryOrderModuleList;

		// Get address of first entry in the linked list
		LIST_ENTRY current{};
		read((std::uint64_t)head->Flink, &current, sizeof(current));

		wprintf(L"[+] All loaded modules:\n");
		int module_count = 0;

		// Iterate through the linked list
		while ((std::uint64_t)current.Flink != (std::uint64_t)head) {
			// Calculate the address of the LDR_DATA_TABLE_ENTRY containing this LIST_ENTRY
			std::uint64_t entry_addr = (std::uint64_t)current.Flink - offsetof(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			// Read the entry
			LDR_DATA_TABLE_ENTRY entry{};
			read(entry_addr, &entry, sizeof(entry));

			// Read module name
			wchar_t name[260] = {};
			USHORT len = entry.BaseDllName.Length;
			if (len > sizeof(name) - sizeof(wchar_t))
				len = sizeof(name) - sizeof(wchar_t);

			// Make sure we have a valid buffer to read from
			if (entry.BaseDllName.Buffer != nullptr && len > 0) {
				read((std::uint64_t)entry.BaseDllName.Buffer, name, len);
				name[len / sizeof(wchar_t)] = L'\0';

				wprintf(L"    [%d] %ws (Base: 0x%llx)\n", module_count++, name, (std::uint64_t)entry.DllBase);

				// Read full path if available
				if (entry.FullDllName.Buffer != nullptr && entry.FullDllName.Length > 0) {
					wchar_t full_path[1024] = {};
					USHORT path_len = entry.FullDllName.Length;
					if (path_len > sizeof(full_path) - sizeof(wchar_t))
						path_len = sizeof(full_path) - sizeof(wchar_t);

					read((std::uint64_t)entry.FullDllName.Buffer, full_path, path_len);
					full_path[path_len / sizeof(wchar_t)] = L'\0';

					wprintf(L"        Path: %ws\n", full_path);
				}
			}

			// Move to the next entry in the list
			read((std::uint64_t)current.Flink, &current, sizeof(current));
		}

		wprintf(L"[+] Total modules: %d\n", module_count);
	}


}