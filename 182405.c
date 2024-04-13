access_check(i_ctx_t *i_ctx_p,
             int access,        /* mask for attrs */
             bool modify)       /* if true, reduce access */
{
    os_ptr op = osp;
    ref *aop;

    switch (r_type(op)) {
        case t_dictionary:
            aop = dict_access_ref(op);
            if (modify) {
                if (!r_has_attrs(aop, access))
                    return_error(gs_error_invalidaccess);
                ref_save(op, aop, "access_check(modify)");
                r_clear_attrs(aop, a_all);
                r_set_attrs(aop, access);
                dict_set_top();
                return 0;
            }
            break;
        case t_array:
        case t_file:
        case t_string:
        case t_mixedarray:
        case t_shortarray:
        case t_astruct:
        case t_device:;
            if (modify) {
                if (!r_has_attrs(op, access))
                    return_error(gs_error_invalidaccess);
                r_clear_attrs(op, a_all);
                r_set_attrs(op, access);
                return 0;
            }
            aop = op;
            break;
        default:
            return_op_typecheck(op);
    }
    return (r_has_attrs(aop, access) ? 1 : 0);
}