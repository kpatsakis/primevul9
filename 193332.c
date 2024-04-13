ves_icall_System_Threading_Thread_VolatileWrite2 (void *ptr, gint16 value)
{
	*((volatile gint16 *) ptr) = value;
}