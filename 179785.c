cin_isdo(char_u *p)
{
    return (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}