vim_strsize(char_u *s)
{
    return vim_strnsize(s, (int)MAXCOL);
}