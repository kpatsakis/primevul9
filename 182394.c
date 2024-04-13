zrcheck(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code = access_check(i_ctx_p, a_read, false);

    if (code >= 0)
        make_bool(op, code), code = 0;
    return code;
}