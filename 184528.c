static void bfuInterrupt(J9VMThread * vmThread)
{
	HANDLE event = (HANDLE) vmThread->sidecarEvent;

	if (event) {
		SetEvent(event);
	}
}