valid_filetype(char_u *val)
{
    char_u *s;

    for (s = val; *s != NUL; ++s)
	if (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)".-_", *s) == NULL)
	    return FALSE;
    return TRUE;
}