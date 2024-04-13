gs_to_exit_with_code(const gs_memory_t *mem, int exit_status, int code)
{
    return gs_main_finit(get_minst_from_memory(mem), exit_status, code);
}