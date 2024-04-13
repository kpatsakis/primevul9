ves_icall_System_Threading_Interlocked_CompareExchange_Long (gint64 *location, gint64 value, gint64 comparand)
{
#if SIZEOF_VOID_P == 8
	return (gint64)InterlockedCompareExchangePointer((gpointer *) location, (gpointer)value, (gpointer)comparand);
#else
	gint64 old;

	mono_interlocked_lock ();
	old = *location;
	if (old == comparand)
		*location = value;
	mono_interlocked_unlock ();
	
	return old;
#endif
}