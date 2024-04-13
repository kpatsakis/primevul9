gs_heap_alloc_struct_array(gs_memory_t * mem, uint num_elements,
                           gs_memory_type_ptr_t pstype, client_name_t cname)
{
    void *ptr =
    gs_heap_alloc_byte_array(mem, num_elements,
                             gs_struct_type_size(pstype), cname);

    if (ptr == 0)
        return 0;
    ((gs_malloc_block_t *) ptr)[-1].type = pstype;
    return ptr;
}