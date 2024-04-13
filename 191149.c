gs_to_exit(const gs_memory_t *mem, int exit_status)
{
    return gs_to_exit_with_code(mem, exit_status, 0);
}