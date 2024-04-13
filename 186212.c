dict_param_enumerate(iparam_list * plist, gs_param_enumerator_t * penum,
                     gs_param_key_t * key, ref_type * type)
{
    ref elt[2];
    int code;
    dict_param_list *const pdlist = (dict_param_list *) plist;
    int index =
    (penum->intval != 0 ? penum->intval : dict_first(&pdlist->dict));

    index = dict_next(&pdlist->dict, index, elt);
    if (index < 0)
        return 1;
    *type = r_type(&elt[1]);
    code = ref_to_key(&elt[0], key, plist);
    penum->intval = index;
    return code;
}