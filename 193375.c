MonoObject * ves_icall_System_Threading_Interlocked_Exchange_Object (MonoObject **location, MonoObject *value)
{
	MonoObject *res;
	res = (MonoObject *) InterlockedExchangePointer((gpointer *) location, value);
	mono_gc_wbarrier_generic_nostore (location);
	return res;
}