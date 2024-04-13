ves_icall_System_Threading_Interlocked_Exchange_Double (gdouble *location, gdouble value)
{
#if SIZEOF_VOID_P == 8
	LongDoubleUnion val, ret;

	val.fval = value;
	ret.ival = (gint64)InterlockedExchangePointer((gpointer *) location, (gpointer)val.ival);

	return ret.fval;
#else
	gdouble res;

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