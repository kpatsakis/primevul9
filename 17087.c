gs_heap_resize_object(gs_memory_t * mem, void *obj, uint new_num_elements,
                      client_name_t cname)
{
    gs_malloc_memory_t *mmem = (gs_malloc_memory_t *) mem;
    gs_malloc_block_t *ptr = (gs_malloc_block_t *) obj - 1;
    gs_memory_type_ptr_t pstype = ptr->type;
    uint old_size = gs_object_size(mem, obj) + sizeof(gs_malloc_block_t);
    uint new_size =
        gs_struct_type_size(pstype) * new_num_elements +
        sizeof(gs_malloc_block_t);
    gs_malloc_block_t *new_ptr;

    if (new_size == old_size)
        return obj;
    if (mmem->monitor)
        gx_monitor_enter(mmem->monitor);	/* Exclusive access */
    new_ptr = (gs_malloc_block_t *) gs_realloc(ptr, old_size, new_size);
    if (new_ptr == 0)
        return 0;
    if (new_ptr->prev)
        new_ptr->prev->next = new_ptr;
    else
        mmem->allocated = new_ptr;
    if (new_ptr->next)
        new_ptr->next->prev = new_ptr;
    new_ptr->size = new_size - sizeof(gs_malloc_block_t);
    mmem->used -= old_size;
    mmem->used += new_size;
    if (mmem->monitor)
        gx_monitor_leave(mmem->monitor);	/* Done with exclusive access */
    if (new_size > old_size)
        gs_alloc_fill((byte *) new_ptr + old_size,
                      gs_alloc_fill_alloc, new_size - old_size);
    return new_ptr + 1;
}