cin_islinecomment(char_u *p)
{
    return (p[0] == '/' && p[1] == '/');
}