f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    fname = tv_get_string(&argvars[0]);
    if (mch_stat((char *)fname, &st) >= 0)
    {
	if (mch_isdir(fname))
	    rettv->vval.v_number = 0;
	else
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    if ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    else
	  rettv->vval.v_number = -1;
}