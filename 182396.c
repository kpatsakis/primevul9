zcvs(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;

    check_write_type(*op, t_string);
    check_op(2);
    code = convert_to_string(imemory, op - 1, op);
    if (code >= 0)
        pop(1);
    return code;
}