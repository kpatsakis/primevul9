find_builtin_term(char_u *term)
{
    struct builtin_term *p;

    p = builtin_termcaps;
    while (p->bt_string != NULL)
    {
	if (p->bt_entry == (int)KS_NAME)
	{
#ifdef UNIX
	    if (STRCMP(p->bt_string, "iris-ansi") == 0 && vim_is_iris(term))
		return p;
	    else if (STRCMP(p->bt_string, "xterm") == 0 && vim_is_xterm(term))
		return p;
	    else
#endif
#ifdef VMS
		if (STRCMP(p->bt_string, "vt320") == 0 && vim_is_vt300(term))
		    return p;
		else
#endif
		  if (STRCMP(term, p->bt_string) == 0)
		    return p;
	}
	++p;
    }
    return p;
}