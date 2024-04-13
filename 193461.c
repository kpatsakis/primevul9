ves_icall_System_Threading_Thread_VolatileWrite8 (void *ptr, gint64 value)
{
	*((volatile gint64 *) ptr) = value;
}