ves_icall_System_Threading_Thread_VolatileWriteObject (void *ptr, void *value)
{
	mono_gc_wbarrier_generic_store (ptr, value);
}