gpointer ves_icall_System_Threading_Interlocked_Exchange_IntPtr (gpointer *location, gpointer value)
{
	return InterlockedExchangePointer(location, value);
}