ref_param_write_init(iparam_list * plist, const ref * pwanted,
                     gs_ref_memory_t *imem)
{
    gs_param_list_init((gs_param_list *)plist, &ref_write_procs,
                       (gs_memory_t *)imem);
    plist->ref_memory = imem;
    if (pwanted == 0)
        make_null(&plist->u.w.wanted);
    else
        plist->u.w.wanted = *pwanted;
    plist->results = 0;
    plist->int_keys = false;
}