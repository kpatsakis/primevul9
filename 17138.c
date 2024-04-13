vim_iswordp(char_u *p)
{
    return vim_iswordp_buf(p, curbuf);
}