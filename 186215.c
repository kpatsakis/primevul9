ref_param_make_string(ref *pe, const void *pvalue, uint i, gs_ref_memory_t *imem)
{
    return ref_param_write_string_value(pe,
                         &((const gs_param_string_array *)pvalue)->data[i],
                                        imem);
}