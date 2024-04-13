ves_icall_System_Threading_Thread_VolatileWrite1 (void *ptr, gint8 value)
{
	*((volatile gint8 *) ptr) = value;
}