stack_param_read(iparam_list * plist, const ref * pkey, iparam_loc * ploc)
{
    stack_param_list *const splist = (stack_param_list *) plist;
    ref_stack_t *pstack = splist->pstack;

    /* This implementation is slow, but it probably doesn't matter. */
    uint index = splist->skip + 1;
    uint count = splist->count;

    for (; count; count--, index += 2) {
        const ref *p = ref_stack_index(pstack, index);

        if (r_has_type(p, t_name) && name_eq(p, pkey)) {
            ploc->pvalue = ref_stack_index(pstack, index - 1);
            ploc->presult = &plist->results[count - 1];
            *ploc->presult = 1;
            return 0;
        }
    }
    return 1;
}