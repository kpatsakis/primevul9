ref_param_read_int_array(gs_param_list * plist, gs_param_name pkey,
                         gs_param_int_array * pvalue)
{
    iparam_list *const iplist = (iparam_list *) plist;
    iparam_loc loc;
    int code = ref_param_read_array(iplist, pkey, &loc);
    int *piv;
    uint size;
    long i;

    if (code != 0)
        return code;
    size = r_size(loc.pvalue);
    piv = (int *)gs_alloc_byte_array(plist->memory, size, sizeof(int),
                                     "ref_param_read_int_array");

    if (piv == 0)
        return_error(gs_error_VMerror);
    for (i = 0; i < size; i++) {
        ref elt;

        array_get(plist->memory, loc.pvalue, i, &elt);
        if (!r_has_type(&elt, t_integer)) {
            code = gs_note_error(gs_error_typecheck);
            break;
        }
        piv[i] = (int)elt.value.intval;
    }
    if (code < 0) {
        gs_free_object(plist->memory, piv, "ref_param_read_int_array");
        return (*loc.presult = code);
    }
    pvalue->data = piv;
    pvalue->size = size;
    pvalue->persistent = true;
    return 0;
}