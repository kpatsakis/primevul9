dict_param_list_read(dict_param_list * plist, const ref * pdict,
                     const ref * ppolicies, bool require_all,
                     gs_ref_memory_t *imem)
{
    iparam_list *const iplist = (iparam_list *) plist;
    uint count;

    if (pdict == 0) {
        plist->u.r.read = empty_param_read;
        count = 0;
    } else {
        check_dict_read(*pdict);
        plist->u.r.read = dict_param_read;
        plist->dict = *pdict;
        count = dict_max_index(pdict) + 1;
    }
    plist->enumerate = dict_param_enumerate;
    return ref_param_read_init(iplist, count, ppolicies, require_all, imem);
}