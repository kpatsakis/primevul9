has_env_var(char_u *p)
{
    for ( ; *p; MB_PTR_ADV(p))
    {
	if (*p == '\\' && p[1] != NUL)
	    ++p;
	else if (vim_strchr((char_u *)
#if defined(MSWIN)
				    "$%"
#else
				    "$"
#endif
					, *p) != NULL)
	    return TRUE;
    }
    return FALSE;
}