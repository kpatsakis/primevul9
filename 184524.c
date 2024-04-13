JVM_RawRealloc(void * ptr, size_t size, const char * callsite)
{
	return j9portLibrary.omrPortLibrary.mem_reallocate_memory(&j9portLibrary.omrPortLibrary, ptr, (UDATA) size, callsite, J9MEM_CATEGORY_SUN_JCL);
}