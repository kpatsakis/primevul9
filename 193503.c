HANDLE ves_icall_System_Threading_Mutex_CreateMutex_internal (MonoBoolean owned, MonoString *name, MonoBoolean *created)
{ 
	HANDLE mutex;
	
	MONO_ARCH_SAVE_REGS;
   
	*created = TRUE;
	
	if (name == NULL) {
		mutex = CreateMutex (NULL, owned, NULL);
	} else {
		mutex = CreateMutex (NULL, owned, mono_string_chars (name));
		
		if (GetLastError () == ERROR_ALREADY_EXISTS) {
			*created = FALSE;
		}
	}

	return(mutex);
}                                                                   