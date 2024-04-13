zexecuteonly(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    check_op(1);
    if (r_has_type(op, t_dictionary))
        return_error(gs_error_typecheck);
    return access_check(i_ctx_p, a_execute, true);
}