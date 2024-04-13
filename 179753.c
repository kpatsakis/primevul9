f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CINDENT
    pos_T	pos;
    linenr_T	lnum;

    if (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	return;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    if (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_c_indent();
	curwin->w_cursor = pos;
    }
    else
# endif
	rettv->vval.v_number = -1;
}