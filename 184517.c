JVM_RawAllocateInCategory(size_t size, const char * callsite, jint category)
{
	return j9portLibrary.omrPortLibrary.mem_allocate_memory(&j9portLibrary.omrPortLibrary, (UDATA) size, (char *) ((callsite == NULL) ? J9_GET_CALLSITE() : callsite), category);
}