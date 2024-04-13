ves_icall_System_Threading_Thread_VolatileRead8 (void *ptr)
{
	return *((volatile gint64 *) (ptr));
}