ref_param_make_float(ref *pe, const void *pvalue, uint i, gs_ref_memory_t *imem)
{
    make_tav(pe, t_real, imemory_new_mask(imem), realval,
             ((const gs_param_float_array *)pvalue)->data[i]);
    return 0;
}