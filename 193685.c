check_help_lang(char_u *arg)
{
    int len = (int)STRLEN(arg);

    if (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	return arg + len - 2;
    }
    return NULL;
}