BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	BOOL rcDllMain = TRUE;
	jvm_dllHandle = hModule;

	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		/* Disable DLL_THREAD_ATTACH and DLL_THREAD_DETACH notifications for WIN32*/
		DisableThreadLibraryCalls(hModule);
	}

	return rcDllMain;
}