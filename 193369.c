gint64 ves_icall_System_Threading_Interlocked_Increment_Long (gint64 *location)
{
	gint64 ret;

	MONO_ARCH_SAVE_REGS;

	mono_interlocked_lock ();

	ret = ++ *location;
	
	mono_interlocked_unlock ();

	
	return ret;
}