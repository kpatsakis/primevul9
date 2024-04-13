static void * __init dt_alloc_memory(u64 size, u64 align)
{
	void *ptr = memblock_alloc(size, align);

	if (!ptr)
		panic("%s: Failed to allocate %llu bytes align=0x%llx\n",
		      __func__, size, align);

	return ptr;
}