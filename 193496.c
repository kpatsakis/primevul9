ves_icall_System_Threading_Interlocked_Exchange_Long (gint64 *location, gint64 value)
{
#if SIZEOF_VOID_P == 8
	return (gint64) InterlockedExchangePointer((gpointer *) location, (gpointer)value);
#else
	gint64 res;

	/* 
	 * According to MSDN, this function is only atomic with regards to the 
	 * other Interlocked functions on 32 bit platforms.
	 */
	mono_interlocked_lock ();
	res = *location;
	*location = value;
	mono_interlocked_unlock ();

	return res;
#endif
}