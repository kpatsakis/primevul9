print_al(const gs_memory_t *mem, const char *label, const active_line * alp)
{
    if (gs_debug_c('F'))
        print_active_line(mem, label, alp);
}