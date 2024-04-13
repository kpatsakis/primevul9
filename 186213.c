array_indexed_param_list_read(dict_param_list * plist, const ref * parray,
                              const ref * ppolicies, bool require_all,
                              gs_ref_memory_t *ref_memory)
{
    iparam_list *const iplist = (iparam_list *) plist;
    int code;

    check_read_type(*parray, t_array);
    plist->u.r.read = array_indexed_param_read;
    plist->dict = *parray;
    code = ref_param_read_init(iplist, r_size(parray), ppolicies,
                               require_all, ref_memory);
    plist->int_keys = true;
    return code;
}