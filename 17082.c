gs_heap_alloc_byte_array(gs_memory_t * mem, uint num_elements, uint elt_size,
                         client_name_t cname)
{
    ulong lsize = (ulong) num_elements * elt_size;

    if (lsize != (uint) lsize)
        return 0;
    return gs_heap_alloc_bytes(mem, (uint) lsize, cname);
}