cleanup_zsubexpr(void)
{
    if (rex.need_clear_zsubexpr)
    {
	if (REG_MULTI)
	{
	    // Use 0xff to set lnum to -1
	    vim_memset(reg_startzpos, 0xff, sizeof(lpos_T) * NSUBEXP);
	    vim_memset(reg_endzpos, 0xff, sizeof(lpos_T) * NSUBEXP);
	}
	else
	{
	    vim_memset(reg_startzp, 0, sizeof(char_u *) * NSUBEXP);
	    vim_memset(reg_endzp, 0, sizeof(char_u *) * NSUBEXP);
	}
	rex.need_clear_zsubexpr = FALSE;
    }
}