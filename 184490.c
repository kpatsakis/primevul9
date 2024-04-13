JVM_RawCalloc(size_t nmemb, size_t size, const char * callsite)
{
	size_t byteSize = nmemb * size;
	void * mem = JVM_RawAllocate(byteSize, callsite);

	if (mem != NULL) {
		memset(mem, 0, byteSize);
	}

	return mem;
}