HANDLE ves_icall_System_Threading_Semaphore_CreateSemaphore_internal (gint32 initialCount, gint32 maximumCount, MonoString *name, MonoBoolean *created)
{ 
	HANDLE sem;
	
	MONO_ARCH_SAVE_REGS;
   
	*created = TRUE;
	
	if (name == NULL) {
		sem = CreateSemaphore (NULL, initialCount, maximumCount, NULL);
	} else {
		sem = CreateSemaphore (NULL, initialCount, maximumCount,
				       mono_string_chars (name));
		
		if (GetLastError () == ERROR_ALREADY_EXISTS) {
			*created = FALSE;
		}
	}

	return(sem);
}                                                                   