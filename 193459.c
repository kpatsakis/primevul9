ves_icall_System_Threading_Interlocked_CompareExchange_T (MonoObject **location, MonoObject *value, MonoObject *comparand)
{
	MonoObject *res;
	res = InterlockedCompareExchangePointer ((gpointer *)location, value, comparand);
	mono_gc_wbarrier_generic_nostore (location);
	return res;
}