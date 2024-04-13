ref_array_param_requested(const iparam_list *iplist, gs_param_name pkey,
                          ref *pvalue, uint size, client_name_t cname)
{
    int code;

    if (!ref_param_requested((const gs_param_list *)iplist, pkey))
        return 0;
    code = gs_alloc_ref_array(iplist->ref_memory, pvalue, a_all, size, cname);
    return (code < 0 ? code : 1);
}