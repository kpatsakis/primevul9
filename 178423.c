vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*expr,
    char_u	*dest,
    int		destlen,
    int		flags)
{
    int		result;
    regexec_T	rex_save;
    int		rex_in_use_save = rex_in_use;

    if (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = NULL;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_line_lbr = TRUE;
    result = vim_regsub_both(source, expr, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    if (rex_in_use)
	rex = rex_save;

    return result;
}