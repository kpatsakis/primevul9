gint32 ves_icall_System_Threading_Interlocked_Increment_Int (gint32 *location)
{
	MONO_ARCH_SAVE_REGS;

	return InterlockedIncrement (location);
}