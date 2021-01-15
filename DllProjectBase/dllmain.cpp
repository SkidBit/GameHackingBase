#include "includes.h"
#include "console.h"

#define show_console 1 //1 = show console ~ 0 = don't show console


DWORD WINAPI MainThread(LPVOID param) {

	// calling GetModuleHandle with NULL returns .exe module
	uintptr_t baseAddressMainMod = (uintptr_t)GetModuleHandle(NULL);
	uintptr_t baseAddressCoreDll = (uintptr_t)GetModuleHandle("Example.dll");

	while (true) {
		//IMPORTANT: if you close the console manually the game/programm you injected this into will crash/close down!
		if (GetAsyncKeyState(VK_INSERT) & 1) { //if Insert is pressed make a short beep and free the console (if used)

			if (show_console) {

				auto temp = GetConsoleWindow();
				FreeConsole();
				PostMessage(temp, WM_QUIT, 0, 0);

			}

			break;
		}

		Sleep(1);
	}

	FreeLibraryAndExitThread((HMODULE)param, 0); //erease traces of your dll (!)
	return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
			if (show_console) console(hModule); //opens console if show_console is set to 1 (how? -> console.h)
		}

		case DLL_PROCESS_DETACH: {
			break;
		}
	}

	return TRUE;
}