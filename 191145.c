gs_pop_integer(gs_main_instance * minst, long *result)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    ref vref;
    int code = pop_value(i_ctx_p, &vref);

    if (code < 0)
        return code;
    check_type_only(vref, t_integer);
    *result = vref.value.intval;
    ref_stack_pop(&o_stack, 1);
    return 0;
}