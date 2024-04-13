bfuThreadEnd(J9HookInterface** vmHooks, UDATA eventNum, void* eventData, void* userData)
{
	J9VMThread* vmThread = ((J9VMThreadDestroyEvent*)eventData)->vmThread;
	HANDLE event = (HANDLE) vmThread->sidecarEvent;

	if (event) {
		CloseHandle(event);
		vmThread->sidecarEvent = NULL;
	}
}