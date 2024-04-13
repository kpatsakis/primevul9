static void *must_realloc(void *ptr, size_t size)
{
	void *old = ptr;
	do {
		ptr = realloc(old, size);
	} while(!ptr);
	return ptr;
}