stack_param_list_read(stack_param_list * plist, ref_stack_t * pstack,
                      uint skip, const ref * ppolicies, bool require_all,
                      gs_ref_memory_t *imem)
{
    iparam_list *const iplist = (iparam_list *) plist;
    uint count = ref_stack_counttomark(pstack);

    if (count == 0)
        return_error(gs_error_unmatchedmark);
    count -= skip + 1;
    if (count & 1)
        return_error(gs_error_rangecheck);
    plist->u.r.read = stack_param_read;
    plist->enumerate = stack_param_enumerate;
    plist->pstack = pstack;
    plist->skip = skip;
    return ref_param_read_init(iplist, count >> 1, ppolicies, require_all, imem);
}