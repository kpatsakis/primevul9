ves_icall_System_Threading_Thread_VolatileWrite4 (void *ptr, gint32 value)
{
	*((volatile gint32 *) ptr) = value;
}