gs_heap_alloc_struct(gs_memory_t * mem, gs_memory_type_ptr_t pstype,
                     client_name_t cname)
{
    void *ptr =
    gs_heap_alloc_bytes(mem, gs_struct_type_size(pstype), cname);

    if (ptr == 0)
        return 0;
    ((gs_malloc_block_t *) ptr)[-1].type = pstype;
    return ptr;
}