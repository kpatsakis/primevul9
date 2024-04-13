gs_malloc_init(void)
{
    gs_malloc_memory_t *malloc_memory_default = gs_malloc_memory_init();
    gs_memory_t *memory_t_default;

    if (malloc_memory_default == NULL)
        return NULL;

    if (gs_lib_ctx_init((gs_memory_t *)malloc_memory_default) != 0)
        return NULL;

#if defined(USE_RETRY_MEMORY_WRAPPER)
    gs_malloc_wrap(&memory_t_default, malloc_memory_default);
#else
    memory_t_default = (gs_memory_t *)malloc_memory_default;
#endif
    memory_t_default->stable_memory = memory_t_default;
    return memory_t_default;
}