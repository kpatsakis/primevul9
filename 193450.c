gboolean ves_icall_System_Threading_Events_SetEvent_internal (HANDLE handle) {
	MONO_ARCH_SAVE_REGS;

	return (SetEvent(handle));
}