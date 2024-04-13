array_param_list_read(array_param_list * plist, ref * bot, uint count,
                      const ref * ppolicies, bool require_all,
                      gs_ref_memory_t *imem)
{
    iparam_list *const iplist = (iparam_list *) plist;

    if (count & 1)
        return_error(gs_error_rangecheck);
    plist->u.r.read = array_param_read;
    plist->enumerate = array_param_enumerate;
    plist->bot = bot;
    plist->top = bot + count;
    return ref_param_read_init(iplist, count, ppolicies, require_all, imem);
}