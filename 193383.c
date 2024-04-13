HANDLE ves_icall_System_Threading_Events_CreateEvent_internal (MonoBoolean manual, MonoBoolean initial, MonoString *name, MonoBoolean *created)
{
	HANDLE event;
	
	MONO_ARCH_SAVE_REGS;

	*created = TRUE;

	if (name == NULL) {
		event = CreateEvent (NULL, manual, initial, NULL);
	} else {
		event = CreateEvent (NULL, manual, initial,
				     mono_string_chars (name));
		
		if (GetLastError () == ERROR_ALREADY_EXISTS) {
			*created = FALSE;
		}
	}
	
	return(event);
}