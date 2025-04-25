#pragma once

namespace decryptions
{
#define LODWORD(x)  (*((DWORD*)&(x)))
#define TEST_BITD(value, bit) (((value) & (1 << (bit))) != 0)
	typedef unsigned int uint32;
#define _DWORD uint32

	inline uintptr_t Il2cppGetHandle(int32_t ObjectHandleID) {

		uint64_t rdi_1 = ObjectHandleID >> 3;
		uint64_t rcx_1 = (ObjectHandleID & 7) - 1;
		uint64_t baseAddr = GameAssembly +  offset::Il2CppGetHandle + rcx_1 * 0x28;
		uint32_t limit = driver::read<uint32_t>(baseAddr + 0x10);
		if (rdi_1 < limit) {
			uintptr_t objAddr = driver::read<uintptr_t>(baseAddr);
			uint32_t bitMask = driver::read<uint32_t>(objAddr + ((rdi_1 >> 5) << 2));
			if (TEST_BITD(bitMask, rdi_1 & 0x1f)) {
				uintptr_t ObjectArray = driver::read<uintptr_t>(baseAddr + 0x8) + (rdi_1 << 3);
				return driver::read<BYTE>(baseAddr + 0x14) > 1
					? driver::read<uintptr_t>(ObjectArray)
					: ~driver::read<uint32_t>(ObjectArray);
			}
		}
		return 0;
	}

	inline uint64_t DecryptNetworkableList(uint64_t Address) {
		__int64* v4; // rdx
		int v5; // r8d
		int v6; // eax
		unsigned int v7; // eax
		__int64 v8; // rcx
		__int64 v22; // [rsp+20h] [rbp-28h] BYREF

		v4 = &v22;
		v22 = driver::read<uint64_t>(Address + 0x18);
		v5 = 2;
		do
		{
			v6 = *(DWORD*)v4;
			v4 = (__int64*)((char*)v4 + 4);
			v7 = (((v6 + 1408399105) << 28) | ((unsigned int)(v6 + 1408399105) >> 4)) + 1449545954;
			*((DWORD*)v4 - 1) = (v7 << 21) | (v7 >> 11);
			--v5;
		} while (v5);
		v8 = v22;

		return Il2cppGetHandle(v22);
	}

	inline uint64_t DecryptBaseNetworkable(uint64_t Address) {
		__int64* v4; // rdx
		int v5; // r8d
		unsigned int v6; // ecx
		unsigned int v7; // eax
		__int64 v8; // rcx
		__int64 v24; // [rsp+20h] [rbp-28h] BYREF

		v4 = &v24;
		v24 = driver::read<uint64_t>(Address + 0x18);
		v5 = 2;
		do
		{
			v6 = *(DWORD*)v4;
			v7 = *(DWORD*)v4;
			v4 = (__int64*)((char*)v4 + 4);
			*((DWORD*)v4 - 1) = (((((v7 << 22) | (v6 >> 10)) + 656138671) << 11) | ((((v7 << 22) | (v6 >> 10)) + 656138671) >> 21))
				- 1352039990;
			--v5;
		} while (v5);
		v8 = v24;

		return Il2cppGetHandle(v24);
	}

	inline uint64_t decrypt_ulong_key(uintptr_t Address)
	{
		__int64* v4; // rdx
		int v5; // r8d
		unsigned int v6; // ecx
		unsigned int v7; // eax
		__int64 v8; // rcx
		__int64 v23; // [rsp+20h] [rbp-28h] BYREF

		v4 = &v23;
		v23 = driver::read<uint64_t>(Address + 0x18);
		v5 = 2;
		do
		{
			v6 = *(_DWORD*)v4;
			v7 = *(_DWORD*)v4;
			v4 = (__int64*)((char*)v4 + 4);
			*((_DWORD*)v4 - 1) = ((((v7 << 23) | (v6 >> 9)) ^ 0x2D9DE272) + 1786976841) ^ 0x80042B5A;
			--v5;
		} while (v5);
		v8 = v23;

		return Il2cppGetHandle(v23);
}
	
	
	inline uint64_t decrypt_inventory_pointer(uint64_t pointer)
	{
		return decrypt_ulong_key(pointer);
	}

	inline uint64_t DecryptPlayerEyes(uint64_t Address) {
		__int64* v4; // rdx
		int v5; // r8d
		int v6; // eax
		__int64 v7; // rcx
		__int64 v8; // rax
		__int64 v22; // [rsp+20h] [rbp-28h] BYREF

		v4 = &v22;
		v22 = driver::read<uint64_t>(Address + 0x18);
		v5 = 2;
		do
		{
			v6 = *(_DWORD*)v4;
			v4 = (__int64*)((char*)v4 + 4);
			*((_DWORD*)v4 - 1) = (((v6 + 657907416) << 7) | ((unsigned int)(v6 + 657907416) >> 25)) - 173559052;
			--v5;
		} while (v5);
		v7 = v22;

		return Il2cppGetHandle(v22);
	}
	inline uint64_t DecryptClActiveItem(uint64_t Address)
	{
		uint64_t* v2; // rdx
		int v3; // r8d
		unsigned int v4; // ecx
		unsigned int v5; // eax
		__int64 result; // rax
		__int64 v7; // rcx
		signed __int32 v8[8]; // [rsp+0h] [rbp-38h] BYREF
		uint64_t v9; // [rsp+20h] [rbp-18h] BYREF

		v2 = &v9;
		v3 = 2;
		v9 = Address;
		do
		{
			v4 = *(_DWORD*)v2;
			v5 = *(_DWORD*)v2;
			v2 = (uint64_t*)((char*)v2 + 4);
			*((_DWORD*)v2 - 1) = (((((v5 << 7) | (v4 >> 25)) - 2064472425) << 10) | ((((v5 << 7) | (v4 >> 25)) - 2064472425) >> 22))
				- 1699390350;
			--v3;
		} while (v3);

		return v9;
	}
}
