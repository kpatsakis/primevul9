gint32 ves_icall_System_Threading_Interlocked_Exchange_Int (gint32 *location, gint32 value)
{
	MONO_ARCH_SAVE_REGS;

	return InterlockedExchange(location, value);
}