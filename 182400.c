ztypenames(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    static const char *const tnames[] = { REF_TYPE_NAME_STRINGS };
    int i;

    check_ostack(t_next_index);
    for (i = 0; i < t_next_index; i++) {
        ref *const rtnp = op + 1 + i;

        if (i >= countof(tnames) || tnames[i] == 0)
            make_null(rtnp);
        else {
            int code = name_enter_string(imemory, tnames[i], rtnp);

            if (code < 0)
                return code;
            r_set_attrs(rtnp, a_executable);
        }
    }
    osp += t_next_index;
    return 0;
}