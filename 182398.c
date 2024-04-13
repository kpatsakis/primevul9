znoaccess(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    check_op(1);
    if (r_has_type(op, t_dictionary)) {
        ref *aop = dict_access_ref(op);

        /* CPSI throws invalidaccess when seting noaccess to a readonly dictionary (CET 13-13-6) : */
        if (!r_has_attrs(aop, a_write)) {
            if (!r_has_attrs(aop, a_read) && !r_has_attrs(aop, a_execute)) {
                /* Already noaccess - do nothing (CET 24-09-1). */
                return 0;
            }
            return_error(gs_error_invalidaccess);
        }

        /* Don't allow removing read access to permanent dictionaries. */
        if (dict_is_permanent_on_dstack(op))
            return_error(gs_error_invalidaccess);
    }
    return access_check(i_ctx_p, 0, true);
}