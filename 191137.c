pop_value(i_ctx_t *i_ctx_p, ref * pvalue)
{
    if (!ref_stack_count(&o_stack))
        return_error(gs_error_stackunderflow);
    *pvalue = *ref_stack_index(&o_stack, 0L);
    return 0;
}