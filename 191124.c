get_op_array(const gs_memory_t *mem, int size)
{
    gs_main_instance *minst = get_minst_from_memory(mem);
    return op_index_op_array_table(minst->i_ctx_p,size);
}