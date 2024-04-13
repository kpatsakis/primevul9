zreadonly(i_ctx_t *i_ctx_p)
{
    return access_check(i_ctx_p, a_readonly, true);
}