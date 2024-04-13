gs_pop_real(gs_main_instance * minst, float *result)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    ref vref;
    int code = pop_value(i_ctx_p, &vref);

    if (code < 0)
        return code;
    switch (r_type(&vref)) {
        case t_real:
            *result = vref.value.realval;
            break;
        case t_integer:
            *result = (float)(vref.value.intval);
            break;
        default:
            return_error(gs_error_typecheck);
    }
    ref_stack_pop(&o_stack, 1);
    return 0;
}