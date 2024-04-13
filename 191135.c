push_value(gs_main_instance *minst, ref * pvalue)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    int code = ref_stack_push(&o_stack, 1);

    if (code < 0)
        return code;
    *ref_stack_index(&o_stack, 0L) = *pvalue;
    return 0;
}