gs_pop_string(gs_main_instance * minst, gs_string * result)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    ref vref;
    int code = pop_value(i_ctx_p, &vref);

    if (code < 0)
        return code;
    switch (r_type(&vref)) {
        case t_name:
            name_string_ref(minst->heap, &vref, &vref);
            code = 1;
            goto rstr;
        case t_string:
            code = (r_has_attr(&vref, a_write) ? 0 : 1);
          rstr:result->data = vref.value.bytes;
            result->size = r_size(&vref);
            break;
        default:
            return_error(gs_error_typecheck);
    }
    ref_stack_pop(&o_stack, 1);
    return code;
}