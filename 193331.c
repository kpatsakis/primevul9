HANDLE ves_icall_System_Threading_Mutex_OpenMutex_internal (MonoString *name,
							    gint32 rights,
							    gint32 *error)
{
	HANDLE ret;
	
	MONO_ARCH_SAVE_REGS;
	
	*error = ERROR_SUCCESS;
	
	ret = OpenMutex (rights, FALSE, mono_string_chars (name));
	if (ret == NULL) {
		*error = GetLastError ();
	}
	
	return(ret);
}