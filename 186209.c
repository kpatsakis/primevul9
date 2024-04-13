ref_param_read_init(iparam_list * plist, uint count, const ref * ppolicies,
                    bool require_all, gs_ref_memory_t *imem)
{
    gs_param_list_init((gs_param_list *)plist, &ref_read_procs,
                       (gs_memory_t *)imem);
    plist->ref_memory = imem;
    if (ppolicies == 0)
        make_null(&plist->u.r.policies);
    else
        plist->u.r.policies = *ppolicies;
    plist->u.r.require_all = require_all;
    plist->count = count;
    plist->results = (int *)
        gs_alloc_byte_array(plist->memory, count, sizeof(int),
                            "ref_param_read_init");

    if (plist->results == 0)
        return_error(gs_error_VMerror);
    memset(plist->results, 0, count * sizeof(int));

    plist->int_keys = false;
    return 0;
}