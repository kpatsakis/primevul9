ves_icall_System_Threading_Interlocked_Add_Long (gint64 *location, gint64 value)
{
#if SIZEOF_VOID_P == 8
	/* Should be implemented as a JIT intrinsic */
	mono_raise_exception (mono_get_exception_not_implemented (NULL));
	return 0;
#else
	gint64 orig;

	mono_interlocked_lock ();
	orig = *location;
	*location = orig + value;
	mono_interlocked_unlock ();

	return orig + value;
#endif
}