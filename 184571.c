JVM_RawReallocInCategory(void * ptr, size_t size, const char * callsite, jint category)
{
	return j9portLibrary.omrPortLibrary.mem_reallocate_memory(&j9portLibrary.omrPortLibrary, ptr, (UDATA) size, callsite, category);
}