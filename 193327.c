gint32 ves_icall_System_Threading_Interlocked_Decrement_Int (gint32 *location)
{
	MONO_ARCH_SAVE_REGS;

	return InterlockedDecrement(location);
}