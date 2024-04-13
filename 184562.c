JVM_RawAllocate(size_t size, const char * callsite)
{
	return j9portLibrary.omrPortLibrary.mem_allocate_memory(&j9portLibrary.omrPortLibrary, (UDATA) size, (char *) ((callsite == NULL) ? J9_GET_CALLSITE() : callsite), J9MEM_CATEGORY_SUN_JCL);
}