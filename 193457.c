gint32 ves_icall_System_Threading_Interlocked_CompareExchange_Int(gint32 *location, gint32 value, gint32 comparand)
{
	MONO_ARCH_SAVE_REGS;

	return InterlockedCompareExchange(location, value, comparand);
}