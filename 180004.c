ves_icall_System_Array_ClearInternal (MonoArray *arr, int idx, int length)
{
	int sz = mono_array_element_size (mono_object_class (arr));
	memset (mono_array_addr_with_size (arr, sz, idx), 0, length * sz);
}