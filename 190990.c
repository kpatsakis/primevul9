call_vim_function(
    char_u      *func,
    int		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    int		ret;
    funcexe_T	funcexe;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;
    ret = call_func(func, -1, rettv, argc, argv, &funcexe);
    if (ret == FAIL)
	clear_tv(rettv);

    return ret;
}