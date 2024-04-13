corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    if (n > 0 && n < curbuf->b_ind_maxparen / 2)
	return curbuf->b_ind_maxparen - (int)n;
    return curbuf->b_ind_maxparen;
}