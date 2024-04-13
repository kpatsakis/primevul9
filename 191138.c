gs_main_alloc_instance(gs_memory_t *mem)
{
    gs_main_instance *minst;
    if (mem == NULL)
        return NULL;

    minst = (gs_main_instance *)gs_alloc_bytes_immovable(mem,
                                                         sizeof(gs_main_instance),
                                                         "init_main_instance");
    if (minst == NULL)
        return NULL;
    memcpy(minst, &gs_main_instance_init_values, sizeof(gs_main_instance_init_values));
    minst->heap = mem;
    mem->gs_lib_ctx->top_of_system = minst;
    return minst;
}