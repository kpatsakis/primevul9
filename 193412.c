ves_icall_System_Threading_Thread_VolatileRead2 (void *ptr)
{
	return *((volatile gint16 *) (ptr));
}