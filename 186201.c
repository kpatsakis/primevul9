stack_param_write(iparam_list * plist, const ref * pkey, const ref * pvalue)
{
    stack_param_list *const splist = (stack_param_list *) plist;
    ref_stack_t *pstack = splist->pstack;
    s_ptr p = pstack->p;

    if (pstack->top - p < 2) {
        int code = ref_stack_push(pstack, 2);

        if (code < 0)
            return code;
        *ref_stack_index(pstack, 1) = *pkey;
        p = pstack->p;
    } else {
        pstack->p = p += 2;
        p[-1] = *pkey;
    }
    *p = *pvalue;
    splist->count++;
    return 0;
}