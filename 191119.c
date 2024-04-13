get_minst_from_memory(const gs_memory_t *mem)
{
    return (gs_main_instance*)mem->gs_lib_ctx->top_of_system;
}