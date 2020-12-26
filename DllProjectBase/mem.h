#pragma once
#include "stdafx.h"
#include <windows.h>
#include <vector>

namespace mem
{
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size);
	char* ScanInternal(char* pattern, char* mask, char* begin, intptr_t size);
	char* ScanModIn(char* pattern, char* mask, std::string modName);
	std::vector<BYTE> detour32(void* src, void* dst, int len);
	std::vector<BYTE> nopBytes(void* address, int amountOfBytesToNop);
	void restoreBytes(void* address, std::vector<BYTE>bytesToRestore);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
}