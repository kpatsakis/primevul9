stack_param_list_write(stack_param_list * plist, ref_stack_t * pstack,
                       const ref * pwanted, gs_ref_memory_t *imem)
{
    plist->u.w.write = stack_param_write;
    ref_param_write_init((iparam_list *) plist, pwanted, imem);
    plist->enumerate = stack_param_enumerate;
    plist->pstack = pstack;
    plist->skip = 0;
    plist->count = 0;
    return 0;
}