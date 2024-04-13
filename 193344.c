ves_icall_System_Threading_Events_CloseEvent_internal (HANDLE handle) {
	MONO_ARCH_SAVE_REGS;

	CloseHandle (handle);
}