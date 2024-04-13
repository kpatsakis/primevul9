array_new_indexed_param_write(iparam_list * iplist, const ref * pkey,
                          const ref * pvalue)
{
    const ref *const arr = &((dict_param_list *)iplist)->dict;
    ref *eltp;

    if (!r_has_type(pkey, t_integer))
        return_error(gs_error_typecheck);
    check_int_ltu(*pkey, r_size(arr));
    store_check_dest(arr, pvalue);
    eltp = arr->value.refs + pkey->value.intval;
    /* ref_assign_new(eltp, pvalue); */
    ref_assign(eltp, pvalue);
    r_set_attrs(eltp, imemory_new_mask(iplist->ref_memory));
    return 0;
}