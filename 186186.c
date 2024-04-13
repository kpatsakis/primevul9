array_indexed_param_read(iparam_list * plist, const ref * pkey, iparam_loc * ploc)
{
    ref *const arr = &((dict_param_list *) plist)->dict;

    check_type(*pkey, t_integer);
    if (pkey->value.intval < 0 || pkey->value.intval >= r_size(arr))
        return 1;
    ploc->pvalue = arr->value.refs + pkey->value.intval;
    ploc->presult = &plist->results[pkey->value.intval];
    *ploc->presult = 1;
    return 0;
}