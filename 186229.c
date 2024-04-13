ref_param_read_string_array(gs_param_list * plist, gs_param_name pkey,
                            gs_param_string_array * pvalue)
{
    iparam_list *const iplist = (iparam_list *) plist;
    iparam_loc loc;
    ref aref;
    int code = ref_param_read_array(iplist, pkey, &loc);
    gs_param_string *psv;
    uint size;
    long i;

    if (code != 0)
        return code;
    size = r_size(loc.pvalue);
    psv = (gs_param_string *)
        gs_alloc_byte_array(plist->memory, size, sizeof(gs_param_string),
                            "ref_param_read_string_array");
    if (psv == 0)
        return_error(gs_error_VMerror);
    aref = *loc.pvalue;
    if (r_has_type(&aref, t_array)) {
        for (i = 0; code >= 0 && i < size; i++) {
            loc.pvalue = aref.value.refs + i;
            code = ref_param_read_string_value(plist->memory, &loc, psv + i);
        }
    } else {
        ref elt;

        loc.pvalue = &elt;
        for (i = 0; code >= 0 && i < size; i++) {
            array_get(plist->memory, &aref, i, &elt);
            code = ref_param_read_string_value(plist->memory, &loc, psv + i);
        }
    }
    if (code < 0) {
        gs_free_object(plist->memory, psv, "ref_param_read_string_array");
        return (*loc.presult = code);
    }
    pvalue->data = psv;
    pvalue->size = size;
    pvalue->persistent = true;
    return 0;
}