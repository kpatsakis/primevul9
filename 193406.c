ves_icall_System_Threading_Thread_ByteArrayToRootDomain (MonoArray *arr)
{
	return byte_array_to_domain (arr, mono_get_root_domain ());
}