ves_icall_System_Threading_Thread_VolatileWriteIntPtr (void *ptr, void *value)
{
	*((volatile void **) ptr) = value;
}