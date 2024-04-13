ves_icall_System_Threading_Thread_VolatileRead1 (void *ptr)
{
	return *((volatile gint8 *) (ptr));
}