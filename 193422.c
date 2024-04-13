MonoObject * ves_icall_System_Threading_Interlocked_CompareExchange_Object (MonoObject **location, MonoObject *value, MonoObject *comparand)
{
	MonoObject *res;
	res = (MonoObject *) InterlockedCompareExchangePointer((gpointer *) location, value, comparand);
	mono_gc_wbarrier_generic_nostore (location);
	return res;
}