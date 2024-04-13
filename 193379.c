ves_icall_System_Threading_Thread_VolatileRead4 (void *ptr)
{
	return *((volatile gint32 *) (ptr));
}