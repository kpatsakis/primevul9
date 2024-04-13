tv_get_lnum(typval_T *argvars)
{
    linenr_T	lnum = -1;

    if (argvars[0].v_type != VAR_STRING || !in_vim9script())
	lnum = (linenr_T)tv_get_number_chk(&argvars[0], NULL);
    if (lnum <= 0 && argvars[0].v_type != VAR_NUMBER)
    {
	int	fnum;
	pos_T	*fp;

	// no valid number, try using arg like line()
	fp = var2fpos(&argvars[0], TRUE, &fnum, FALSE);
	if (fp != NULL)
	    lnum = fp->lnum;
    }
    return lnum;
}