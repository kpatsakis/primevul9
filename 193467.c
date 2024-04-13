ves_icall_System_Threading_Thread_VolatileReadIntPtr (void *ptr)
{
	return (void *)  *((volatile void **) ptr);
}