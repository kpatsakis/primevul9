makefoldset(FILE *fd)
{
    if (put_setstring(fd, "setlocal", "fdm", &curwin->w_p_fdm, FALSE) == FAIL
# ifdef FEAT_EVAL
	    || put_setstring(fd, "setlocal", "fde", &curwin->w_p_fde, FALSE)
								       == FAIL
# endif
	    || put_setstring(fd, "setlocal", "fmr", &curwin->w_p_fmr, FALSE)
								       == FAIL
	    || put_setstring(fd, "setlocal", "fdi", &curwin->w_p_fdi, FALSE)
								       == FAIL
	    || put_setnum(fd, "setlocal", "fdl", &curwin->w_p_fdl) == FAIL
	    || put_setnum(fd, "setlocal", "fml", &curwin->w_p_fml) == FAIL
	    || put_setnum(fd, "setlocal", "fdn", &curwin->w_p_fdn) == FAIL
	    || put_setbool(fd, "setlocal", "fen", curwin->w_p_fen) == FAIL
	    )
	return FAIL;

    return OK;
}