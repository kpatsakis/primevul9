gs_heap_object_type(const gs_memory_t * mem, const void *ptr)
{
    return ((const gs_malloc_block_t *)ptr)[-1].type;
}