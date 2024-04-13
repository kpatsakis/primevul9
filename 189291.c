check_cedit(void)
{
    int n;

    if (*p_cedit == NUL)
	cedit_key = -1;
    else
    {
	n = string_to_key(p_cedit);
	if (vim_isprintc(n))
	    return e_invarg;
	cedit_key = n;
    }
    return NULL;
}