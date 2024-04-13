fill_submatch_list(int argc UNUSED, typval_T *argv, int argskip, int argcount)
{
    listitem_T	*li;
    int		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    if (argcount == argskip)
	// called function doesn't take a submatches argument
	return argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    for (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	if (s == NULL || rsm.sm_match->endp[i] == NULL)
	    s = NULL;
	else
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_next;
    }
    return argskip + 1;
}