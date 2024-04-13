ves_icall_System_Threading_Interlocked_CompareExchange_Double (gdouble *location, gdouble value, gdouble comparand)
{
#if SIZEOF_VOID_P == 8
	LongDoubleUnion val, comp, ret;

	val.fval = value;
	comp.fval = comparand;
	ret.ival = (gint64)InterlockedCompareExchangePointer((gpointer *) location, (gpointer)val.ival, (gpointer)comp.ival);

	return ret.fval;
#else
	gdouble old;

	mono_interlocked_lock ();
	old = *location;
	if (old == comparand)
		*location = value;
	mono_interlocked_unlock ();

	return old;
#endif
}