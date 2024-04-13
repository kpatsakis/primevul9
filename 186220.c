ref_param_make_int(ref *pe, const void *pvalue, uint i, gs_ref_memory_t *imem)
{
    make_tav(pe, t_integer, imemory_new_mask(imem), intval,
             ((const gs_param_int_array *)pvalue)->data[i]);
    return 0;
}