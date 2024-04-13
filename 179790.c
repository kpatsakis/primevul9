cin_iscomment(char_u *p)
{
    return (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}