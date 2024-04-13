ves_icall_System_Threading_Interlocked_Read_Long (gint64 *location)
{
#if SIZEOF_VOID_P == 8
	/* 64 bit reads are already atomic */
	return *location;
#else
	gint64 res;

	mono_interlocked_lock ();
	res = *location;
	mono_interlocked_unlock ();

	return res;
#endif
}