dict_param_read(iparam_list * plist, const ref * pkey, iparam_loc * ploc)
{
    ref const *spdict = &((dict_param_list *) plist)->dict;
    int code = dict_find(spdict, pkey, &ploc->pvalue);

    if (code != 1)
        return 1;
    ploc->presult =
        &plist->results[dict_value_index(spdict, ploc->pvalue)];
    *ploc->presult = 1;
    return 0;
}