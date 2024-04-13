vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
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

    rex.reg_match = NULL;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_line_count - lnum;
    rex.reg_line_lbr = FALSE;
    result = vim_regsub_both(source, NULL, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    if (rex_in_use)
	rex = rex_save;

    return result;
}