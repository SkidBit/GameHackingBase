#include "stdafx.h"
#include "mem.h"
#include "ntdll.h"

void mem::restoreBytes(void* address, std::vector<BYTE>bytesToRestore) {

	DWORD curProtection;
	VirtualProtect(address, bytesToRestore.size(), PAGE_EXECUTE_READWRITE, &curProtection);

	memcpy(address, bytesToRestore.data(), bytesToRestore.size());

	DWORD temp;
	VirtualProtect(address, bytesToRestore.size(), curProtection, &temp);
}

std::vector<BYTE> mem::nopBytes(void* address, int amountOfBytesToNop) {


	DWORD curProtection;
	VirtualProtect(address, amountOfBytesToNop, PAGE_EXECUTE_READWRITE, &curProtection);

	std::vector<BYTE> originalBytes;
	BYTE currentByte;

	for (int i = 0; i < amountOfBytesToNop; i++) {
		currentByte = *(BYTE*)((BYTE*)address + i);
		originalBytes.push_back(currentByte);
	}

	memset(address, 0x90, amountOfBytesToNop);

	VirtualProtect(address, amountOfBytesToNop, curProtection, &curProtection);

	return originalBytes;
}

std::vector<BYTE> mem::detour32(void* src, void* dst, int len)
{
	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	// return original bytes of function for restoring
	// len < 5 does not work!
	std::vector<BYTE> originalBytes;
	BYTE currentByte;

	for (int i = 0; i < len; i++) {
		currentByte = *(BYTE*)((BYTE*)src + i);
		originalBytes.push_back(currentByte);
	}

	memset(src, 0x90, len);

	uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

	*(BYTE*)src = 0xE9;
	*(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(src, len, curProtection, &temp);

	return originalBytes;
}

char* TO_CHAR(wchar_t* string)
{
	size_t len = wcslen(string) + 1;
	char* c_string = new char[len];
	size_t numCharsRead;
	wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
	return c_string;
}

PEB* GetPEB()
{
#ifdef _WIN64
	PEB* peb = (PEB*)__readgsword(0x60);

#else
	PEB* peb = (PEB*)__readfsdword(0x30);
#endif

	return peb;
}

LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name)
{
	LDR_DATA_TABLE_ENTRY* ldr = nullptr;

	PEB* peb = GetPEB();

	LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList;

	LIST_ENTRY curr = head;

	while (curr.Flink != head.Blink)
	{
		LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (mod->FullDllName.Buffer)
		{
			char* cName = TO_CHAR(mod->BaseDllName.Buffer);

			if (_stricmp(cName, name.c_str()) == 0)
			{
				ldr = mod;
				break;
			}
			delete[] cName;
		}
		curr = *curr.Flink;
	}
	return ldr;
}

char* mem::ScanModIn(char* pattern, char* mask, std::string modName)
{
	LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(modName);

	char* match = mem::ScanInternal(pattern, mask, (char*)ldr->DllBase, ldr->SizeOfImage);

	return match;
}


char* mem::ScanInternal(char* pattern, char* mask, char* begin, intptr_t size)
{
	char* match{ nullptr };
	MEMORY_BASIC_INFORMATION mbi{};

	for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
	{
		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

		match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

		if (match != nullptr)
		{
			break;
		}
	}
	return match;
}



char* mem::ScanBasic(char* pattern, char* mask, char* begin, intptr_t size)
{
	intptr_t patternLen = strlen(mask);

	for (int i = 0; i < size; i++)
	{
		bool found = true;
		for (int j = 0; j < patternLen; j++)
		{
			if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (begin + i);
		}
	}
	return nullptr;
}



void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void mem::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}