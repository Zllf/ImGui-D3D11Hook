#include "dllmain.h"

ScriptHook t;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: // 进程加载
		CreateThread(NULL, 0, ThreadFunc, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH: // 线程加载
		break;
	case DLL_PROCESS_DETACH: // 进程卸载
		t.Release();
		break;
	case DLL_THREAD_DETACH: // 线程卸载
		break;
	}
	return TRUE;
}

DWORD WINAPI ThreadFunc(LPVOID)
{
	t.Initialize();
	return 0;
}