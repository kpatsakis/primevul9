array_param_read(iparam_list * plist, const ref * pkey, iparam_loc * ploc)
{
    ref *bot = ((array_param_list *) plist)->bot;
    ref *ptr = bot;
    ref *top = ((array_param_list *) plist)->top;

    for (; ptr < top; ptr += 2) {
        if (r_has_type(ptr, t_name) && name_eq(ptr, pkey)) {
            ploc->pvalue = ptr + 1;
            ploc->presult = &plist->results[ptr - bot];
            *ploc->presult = 1;
            return 0;
        }
    }
    return 1;
}