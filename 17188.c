get_past_head(char_u *path)
{
    char_u  *retval;

#if defined(MSWIN)
    // may skip "c:"
    if (isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    else
	retval = path;
#else
# if defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    if (retval == NULL)
	retval = path;
# else	// Unix
    retval = path;
# endif
#endif

    while (vim_ispathsep(*retval))
	++retval;

    return retval;
}