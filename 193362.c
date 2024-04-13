gint32 ves_icall_System_Threading_Semaphore_ReleaseSemaphore_internal (HANDLE handle, gint32 releaseCount, MonoBoolean *fail)
{ 
	gint32 prevcount;
	
	MONO_ARCH_SAVE_REGS;

	*fail = !ReleaseSemaphore (handle, releaseCount, &prevcount);

	return (prevcount);
}