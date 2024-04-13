f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    if (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	if (in_vim9script())
	    (void) check_for_string_arg(argvars, 0);
	return;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    if (cwd != NULL)
    {
	if (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
#endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    if (curwin->w_localdir != NULL)
	scope = CDSCOPE_WINDOW;
    else if (curtab->tp_localdir != NULL)
	scope = CDSCOPE_TABPAGE;

    if (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}