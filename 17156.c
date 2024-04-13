vim_isAbsName(char_u *name)
{
    return (path_with_url(name) != 0 || mch_isFullName(name));
}