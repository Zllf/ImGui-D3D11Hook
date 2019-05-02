#include "dllmain.h"

ScriptHook t;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: // ���̼���
		CreateThread(NULL, 0, ThreadFunc, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH: // �̼߳���
		break;
	case DLL_PROCESS_DETACH: // ����ж��
		t.Release();
		break;
	case DLL_THREAD_DETACH: // �߳�ж��
		break;
	}
	return TRUE;
}

DWORD WINAPI ThreadFunc(LPVOID)
{
	t.Initialize();
	return 0;
}