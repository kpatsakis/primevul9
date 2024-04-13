zcvn(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    check_read_type(*op, t_string);
    return name_from_string(imemory, op, op);
}