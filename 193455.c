ves_icall_System_Threading_Thread_ByteArrayToCurrentDomain (MonoArray *arr)
{
	return byte_array_to_domain (arr, mono_domain_get ());
}