gs_malloc_wrapped_contents(gs_memory_t *wrapped)
{
#ifdef USE_RETRY_MEMORY_WRAPPER
    gs_memory_retrying_t *rmem = (gs_memory_retrying_t *)wrapped;

    return (gs_malloc_memory_t *)gs_memory_retrying_target(rmem);
#else /* retrying */
    return (gs_malloc_memory_t *)wrapped;
#endif /* retrying */
}