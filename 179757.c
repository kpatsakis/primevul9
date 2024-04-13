cin_isif(char_u *p)
{
 return (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}