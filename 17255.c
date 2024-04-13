vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    return (*p == NUL || *p == '\r' || *p == '\n');
}