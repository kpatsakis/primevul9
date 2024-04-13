JVM_RawFree(void * ptr)
{
	j9portLibrary.omrPortLibrary.mem_free_memory(&j9portLibrary.omrPortLibrary, ptr);
}