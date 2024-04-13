gs_malloc_release(gs_memory_t *mem)
{
    gs_malloc_memory_t * malloc_memory_default;

    if (mem == NULL)
        return;

    /* Use gs_debug['a'] if gs_debug[':'] is set to dump the heap stats */
    if (gs_debug[':']) {
        void *temp;
        char save_debug_a = gs_debug['a'];

        gs_debug['a'] = 1;
        temp = (char *)gs_alloc_bytes_immovable(mem, 8, "gs_malloc_release");
        gs_debug['a'] = save_debug_a;
        gs_free_object(mem, temp, "gs_malloc_release");
    }

#ifdef USE_RETRY_MEMORY_WRAPPER
    malloc_memory_default = gs_malloc_unwrap(mem);
#else
    malloc_memory_default = (gs_malloc_memory_t *)mem;
#endif
    gs_lib_ctx_fin((gs_memory_t *)malloc_memory_default);

    gs_malloc_memory_release(malloc_memory_default);
}