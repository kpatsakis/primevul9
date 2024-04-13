ref_param_read_float_array(gs_param_list * plist, gs_param_name pkey,
                           gs_param_float_array * pvalue)
{
    iparam_list *const iplist = (iparam_list *) plist;
    iparam_loc loc;
    ref aref, elt;
    int code = ref_param_read_array(iplist, pkey, &loc);
    float *pfv;
    uint size;
    long i;

    if (code != 0)
        return code;
    size = r_size(loc.pvalue);
    pfv = (float *)gs_alloc_byte_array(plist->memory, size, sizeof(float),
                                       "ref_param_read_float_array");

    if (pfv == 0)
        return_error(gs_error_VMerror);
    aref = *loc.pvalue;
    loc.pvalue = &elt;
    for (i = 0; code >= 0 && i < size; i++) {
        array_get(plist->memory, &aref, i, &elt);
        code = float_param(&elt, pfv + i);
    }
    if (code < 0) {
        gs_free_object(plist->memory, pfv, "ref_read_float_array_param");
        return (*loc.presult = code);
    }
    pvalue->data = pfv;
    pvalue->size = size;
    pvalue->persistent = true;
    return 0;
}