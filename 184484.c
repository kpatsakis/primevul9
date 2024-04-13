static void bfuClearInterrupt(J9VMThread * vmThread)
{
	HANDLE event = (HANDLE) vmThread->sidecarEvent;

	if (event) {
		ResetEvent(event);
	}
}