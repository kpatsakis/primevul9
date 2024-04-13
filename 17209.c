vim_isalpha(int c)
{
    return vim_islower(c) || vim_isupper(c);
}