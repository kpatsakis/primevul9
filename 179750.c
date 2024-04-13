cin_nocode(char_u *s)
{
    return *cin_skipcomment(s) == NUL;
}