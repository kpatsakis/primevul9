getvcol_nolist(pos_T *posp)
{
    int		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    if (posp->coladd)
	getvvcol(curwin, posp, NULL, &vcol, NULL);
    else
	getvcol(curwin, posp, NULL, &vcol, NULL);
    curwin->w_p_list = list_save;
    return vcol;
}