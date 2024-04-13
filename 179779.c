cin_isbreak(char_u *p)
{
    return (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
}