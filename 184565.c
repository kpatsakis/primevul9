JVM_RawCallocInCategory(size_t nmemb, size_t size, const char * callsite, jint category)
{
	size_t byteSize = nmemb * size;
	void * mem = JVM_RawAllocateInCategory(byteSize, callsite, category);

	if (mem != NULL) {
		memset(mem, 0, byteSize);
	}

	return mem;
}