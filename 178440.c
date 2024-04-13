reg_submatch_list(int no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    int		i;
    list_T	*list;
    int		error = FALSE;

    if (!can_f_submatch || no < 0)
	return NULL;

    if (rsm.sm_match == NULL)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	if (slnum < 0 || elnum < 0)
	    return NULL;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	if (list == NULL)
	    return NULL;

	s = reg_getline_submatch(slnum) + scol;
	if (slnum == elnum)
	{
	    if (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	else
	{
	    if (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    for (i = 1; i < elnum - slnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		if (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    if (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    else
    {
	s = rsm.sm_match->startp[no];
	if (s == NULL || rsm.sm_match->endp[no] == NULL)
	    return NULL;
	list = list_alloc();
	if (list == NULL)
	    return NULL;
	if (list_append_string(list, s,
				 (int)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    if (error)
    {
	list_free(list);
	return NULL;
    }
    ++list->lv_refcount;
    return list;
}