gpointer ves_icall_System_Threading_Interlocked_CompareExchange_IntPtr(gpointer *location, gpointer value, gpointer comparand)
{
	return InterlockedCompareExchangePointer(location, value, comparand);
}