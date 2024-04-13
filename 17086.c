gs_heap_free_object(gs_memory_t * mem, void *ptr, client_name_t cname)
{
    gs_malloc_memory_t *mmem = (gs_malloc_memory_t *) mem;
    gs_malloc_block_t *bp;
    gs_memory_type_ptr_t pstype;
    struct_proc_finalize((*finalize));

    if_debug3m('a', mem, "[a-]gs_free(%s) 0x%lx(%u)\n",
               client_name_string(cname), (ulong) ptr,
               (ptr == 0 ? 0 : ((gs_malloc_block_t *) ptr)[-1].size));
    if (ptr == 0)
        return;
    pstype = ((gs_malloc_block_t *) ptr)[-1].type;
    finalize = pstype->finalize;
    if (finalize != 0) {
        if_debug3m('u', mem, "[u]finalizing %s 0x%lx (%s)\n",
                   struct_type_name_string(pstype),
                   (ulong) ptr, client_name_string(cname));
        (*finalize) (mem, ptr);
    }
    if (mmem->monitor)
        gx_monitor_enter(mmem->monitor);	/* Exclusive access */
    /* Previously, we used to search through every allocated block to find
     * the block we are freeing. This gives us safety in that an attempt to
     * free an unallocated block will not go wrong. This does radically
     * slow down frees though, so we replace it with this simpler code; we
     * now assume that the block is valid, and hence avoid the search.
     */
#if 1
    bp = &((gs_malloc_block_t *)ptr)[-1];
    if (bp->prev)
        bp->prev->next = bp->next;
    if (bp->next)
        bp->next->prev = bp->prev;
    if (bp == mmem->allocated) {
        mmem->allocated = bp->next;
        mmem->allocated->prev = NULL;
    }
    mmem->used -= bp->size + sizeof(gs_malloc_block_t);
    if (mmem->monitor)
        gx_monitor_leave(mmem->monitor);	/* Done with exclusive access */
    gs_alloc_fill(bp, gs_alloc_fill_free,
                  bp->size + sizeof(gs_malloc_block_t));
    free(bp);
#else
    bp = mmem->allocated; /* If 'finalize' releases a memory,
                             this function could be called recursively and
                             change mmem->allocated. */
    if (ptr == bp + 1) {
        mmem->allocated = bp->next;
        mmem->used -= bp->size + sizeof(gs_malloc_block_t);

        if (mmem->allocated)
            mmem->allocated->prev = 0;
        if (mmem->monitor)
            gx_monitor_leave(mmem->monitor);	/* Done with exclusive access */
        gs_alloc_fill(bp, gs_alloc_fill_free,
                      bp->size + sizeof(gs_malloc_block_t));
        free(bp);
    } else {
        gs_malloc_block_t *np;

        /*
         * bp == 0 at this point is an error, but we'd rather have an
         * error message than an invalid access.
         */
        if (bp) {
            for (; (np = bp->next) != 0; bp = np) {
                if (ptr == np + 1) {
                    bp->next = np->next;
                    if (np->next)
                        np->next->prev = bp;
                    mmem->used -= np->size + sizeof(gs_malloc_block_t);
                    if (mmem->monitor)
                        gx_monitor_leave(mmem->monitor);	/* Done with exclusive access */
                    gs_alloc_fill(np, gs_alloc_fill_free,
                                  np->size + sizeof(gs_malloc_block_t));
                    free(np);
                    return;
                }
            }
        }
        if (mmem->monitor)
            gx_monitor_leave(mmem->monitor);	/* Done with exclusive access */
        lprintf2("%s: free 0x%lx not found!\n",
                 client_name_string(cname), (ulong) ptr);
        free((char *)((gs_malloc_block_t *) ptr - 1));
    }
#endif
}