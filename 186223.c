ref_param_make_name(ref * pe, const void *pvalue, uint i, gs_ref_memory_t *imem)
{
    return ref_param_write_name_value((const gs_memory_t *)imem, pe,
                         &((const gs_param_string_array *)pvalue)->data[i]);
}