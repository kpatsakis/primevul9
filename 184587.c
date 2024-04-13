bfuThreadStart(J9HookInterface** vmHooks, UDATA eventNum, void* eventData, void* userData)
{
	J9VMThreadCreatedEvent* event = eventData;
	if (event->continueInitialization) {
		HANDLE win32Event = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (win32Event == NULL) {
			/* abort the creation of this thread */
			event->continueInitialization = 0;
		} else {
			event->vmThread->sidecarEvent = win32Event;
		}
	}
}