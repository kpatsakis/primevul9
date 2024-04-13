zcvlit(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    ref *aop;

    check_op(1);
    aop = ACCESS_REF(op);
    r_clear_attrs(aop, a_executable);
    return 0;
}