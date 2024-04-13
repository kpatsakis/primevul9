cleanup_subexpr(void)
{
    if (rex.need_clear_subexpr)
    {
	if (REG_MULTI)
	{
	    // Use 0xff to set lnum to -1
	    vim_memset(rex.reg_startpos, 0xff, sizeof(lpos_T) * NSUBEXP);
	    vim_memset(rex.reg_endpos, 0xff, sizeof(lpos_T) * NSUBEXP);
	}
	else
	{
	    vim_memset(rex.reg_startp, 0, sizeof(char_u *) * NSUBEXP);
	    vim_memset(rex.reg_endp, 0, sizeof(char_u *) * NSUBEXP);
	}
	rex.need_clear_subexpr = FALSE;
    }
}