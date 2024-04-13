f_screenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    int		row = 0;
    int		scol = 0, ccol = 0, ecol = 0;

    if (rettv_dict_alloc(rettv) != OK)
	return;
    dict = rettv->vval.v_dict;

    if (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	return;

    wp = find_win_by_nr_or_id(&argvars[0]);
    if (wp == NULL)
	return;

    pos.lnum = tv_get_number(&argvars[1]);
    pos.col = tv_get_number(&argvars[2]) - 1;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &row, &scol, &ccol, &ecol);

    dict_add_number(dict, "row", row);
    dict_add_number(dict, "col", scol);
    dict_add_number(dict, "curscol", ccol);
    dict_add_number(dict, "endcol", ecol);
}