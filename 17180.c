vim_isNormalIDc(int c)
{
    return ASCII_ISALNUM(c) || c == '_';
}