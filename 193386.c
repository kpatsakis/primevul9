MonoBoolean ves_icall_System_Threading_Mutex_ReleaseMutex_internal (HANDLE handle ) { 
	MONO_ARCH_SAVE_REGS;

	return(ReleaseMutex (handle));
}