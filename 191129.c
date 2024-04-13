gs_pop_boolean(gs_main_instance * minst, bool * result)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    ref vref;
    int code = pop_value(i_ctx_p, &vref);

    if (code < 0)
        return code;
    check_type_only(vref, t_boolean);
    *result = vref.value.boolval;
    ref_stack_pop(&o_stack, 1);
    return 0;
}