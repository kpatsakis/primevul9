gs_malloc_unwrap(gs_memory_t *wrapped)
{
#ifdef USE_RETRY_MEMORY_WRAPPER
    gs_memory_retrying_t *rmem = (gs_memory_retrying_t *)wrapped;
    gs_memory_t *contents = gs_memory_retrying_target(rmem);

    gs_free_object(wrapped rmem, "gs_malloc_unwrap(retrying)");
    return (gs_malloc_memory_t *)contents;
#else
    return (gs_malloc_memory_t *)wrapped;
#endif
}