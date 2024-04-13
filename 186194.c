ref_param_write_string_value(ref * pref, const gs_param_string * pvalue,
                             gs_ref_memory_t *imem)
{
    const byte *pdata = pvalue->data;
    uint n = pvalue->size;

    if (pvalue->persistent)
        make_const_string(pref, a_readonly | avm_foreign, n, pdata);
    else {
        byte *pstr = gs_alloc_string((gs_memory_t *)imem, n,
                                     "ref_param_write_string");

        if (pstr == 0)
            return_error(gs_error_VMerror);
        memcpy(pstr, pdata, n);
        make_string(pref, a_readonly | imemory_space(imem), n, pstr);
    }
    return 0;
}