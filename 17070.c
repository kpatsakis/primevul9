gs_heap_status(gs_memory_t * mem, gs_memory_status_t * pstat)
{
    gs_malloc_memory_t *mmem = (gs_malloc_memory_t *) mem;

    pstat->allocated = mmem->used + heap_available();
    pstat->used = mmem->used;
    pstat->is_thread_safe = true;	/* this allocator has a mutex (monitor) and IS thread safe */
}